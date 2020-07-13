#include "Visualisation_Splinters.h"
#include "Mesh.h"
#include "MeshGenerator.h"
/*
	Cellular Automata with Graphs:
		-> [Int] :: {Parents, Siblings, Children, Parent's Siblings}


		-> Volume grows and shrinks the tree.

		FFT:
		-> FFT divided into bins, bins cause oscillation.
			-> Log the bins first and interpolate.
			-> 2 Nodes in first layer of tree have oscillation amplitude determined by
				amplitude of first and second half of FFT,
					-> Recursively move down the tree and do the same.
			-> Where the frequency components are distributed should change the number of branches,
			-> Do something with the phase component (this could move the centre of oscillation)
		Calculating Oscillation:
			-> Frequency of oscillation is prop to length in physics way.
			-> Dampening needs to occur. A damping constant needs to be applied, and this
				should be found experimentally. It might need to vary with node depth.
			-> Oscillation can be circular, or ellipsoid, and in a limit, flat.
				->
		Colour:
			-> We have H,S,V.
				-> We have amplitude, FFT amplitude, FFT phase. These should be mapped to the colour.
*/


Visualisation_Splinters::Visualisation_Splinters()
	: m_seed{ 0 },
	m_roots{},
	m_objectShader{ nullptr }
{
}

void Visualisation_Splinters::activate()
{
	m_objectShader = new Shader{ "../Shaders/Cubes_Vertex.vs", "../Shaders/Cubes_ObjectFragment.fs" };


	SplinterNode* root = new SplinterNode{ glm::vec3{0.0,0.0,0.0},nullptr,1.5 };
	m_roots.push_back(root);
	root->addChild(glm::vec3{ 0.0,0.0,10.0 }, 0.5);
	root->addChild(glm::vec3{ 0.0,10.0,0.0 }, 0.5);
	SplinterNode* c3 = root->addChild(glm::vec3{ 30.0,10.0,0.0 }, 0.2);
	c3->addChild(glm::vec3{ 35.0,15.0,2.0 }, 0.2f);
	c3->addChild(glm::vec3{ 35.0,25.0,-8.0 }, 0.2f);

	MetaDistribution<double> direction{ 0.5f,0.1f,0.001f,0.3f };
	std::normal_distribution<double> directionX{ direction.createDistribution() };
	std::normal_distribution<double> directionY{ direction.createDistribution() };
	std::normal_distribution<double> directionZ{ direction.createDistribution() };

	MeshGenerator::generateUnknownSurface(m_testMesh,1.5,1.5,2.5);

}

void Visualisation_Splinters::deactivate()
{
	delete m_objectShader;
	m_objectShader = nullptr;
}
#include "Buffer.h"

void Visualisation_Splinters::processSamples(const Buffer& buf, unsigned samples)
{
	auto fft{ buf.fft() };

	auto fftLeft{ fft.front() };
	size_t depth = (double)std::sqrt((double)fft.size());

	SplinterNode* root = new SplinterNode{ glm::vec3{0.0} };
	root->makeBranches(std::vector<unsigned>{}, depth);

	// splinter needs pos, centrepos, velocity.




}

void Visualisation_Splinters::renderFrame(const Camera& camera, const Timestep& t)
{
	glm::mat4 projection{ camera.projectionMatrix() };
	m_objectShader->setMat4("projection", projection);

	// camera/view transformation
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model{ 1.0f };

	m_objectShader->setVec3("lightColour", glm::vec3{ 1.0f,1.0f,1.0f });
	m_objectShader->setVec3("objectColour", glm::vec3{ 0.1f,0.15f,0.31f });
	m_objectShader->setMat4("view", view);
	m_objectShader->setVec3("lightPos", glm::vec3{ 2.0,2.0,2.0 });
	m_objectShader->setVec3("viewPos", camera.m_position);
	m_objectShader->setMat4("model", model);

	m_objectShader->use();
	m_testMesh.draw(m_objectShader);

	//for (SplinterNode* root : m_roots)
	//{
	//	std::vector<Mesh> meshes{ root->build() };
	//	m_objectShader->use();
	//	for (Mesh mesh : meshes)
	//		mesh.draw(m_objectShader);
	//}
}
#include <imgui/imgui.h>
void Visualisation_Splinters::drawInterface()
{
	unsigned depth{ 3 };
	unsigned siblings{ 3 };

	ImGui::SliderFloat3("A B C", (float*)&abc, 0.0f, 10.0f);
	if (abc != abcOrig)
	{
		MeshGenerator::generateUnknownSurface(m_testMesh, abc.x, abc.y, abc.z);
		abcOrig = abc;
	}
}
  
void SplinterNode::makeBranches(std::vector<unsigned>& branches, int depth)
{
	// Don't be responsible for geometry here, just node layout. 
	// For geometry: need scale. need to find vector from parent to this, and vector from this to sibling.
	unsigned layerBranches = branches.front();
	branches.erase(branches.begin()); // <- slow, todo is swap with another container.
	--depth;
	if (depth > 0)
	{
		std::vector<unsigned> remaining{ branches };
		for (unsigned i = 0; i < layerBranches; ++i)
		{
			SplinterNode* child{ new SplinterNode{m_position,this} };
			child->makeBranches(remaining, depth);
			m_children.push_back(child);
		}
	}
}

void SplinterNode::initialiseGeometry()
{
	glm::vec3 parentOrientation{0.0,0.0,1.0};
	if (m_parent != nullptr)
		if (m_parent->m_parent != nullptr)
			parentOrientation = glm::normalize(m_parent->m_position - m_parent->m_parent->m_position);
	
	for (unsigned i = 0; i < m_children.size(); ++i)
	{

	}
}

std::vector<Mesh> SplinterNode::build(Mesh m)
{
	std::vector<Mesh> result{};
	Mesh ball{};
	MeshGenerator::generateSphere(10, ball);
	ball.scale(glm::vec3{ m_scale });
	ball.translate(m_position);
	result.push_back(ball);

	float cylinderFraction{ 0.4 };

	for (SplinterNode* child : m_children)
	{
		Mesh connector{};
		MeshGenerator::generateCylinder(connector, m_position, child->m_position, 10, m_scale * cylinderFraction,
			child->m_scale * cylinderFraction);
		connector.translate(m_position);
		result.push_back(connector);
		std::vector<Mesh> childMeshes{ child->build() };
		result.insert(result.end(), childMeshes.begin(), childMeshes.end());
	}
	return result;
}
