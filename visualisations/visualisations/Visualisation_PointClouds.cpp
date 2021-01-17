#include "Visualisation_PointClouds.h"

#include "../graphics/Shader.h"
#include "../audio/Buffer.h"
#include "../maths/MetaDistribution.h"

#include <imgui.h>
#include <GLFW\glfw3.h>
#include <complex>
#include <vector>

Visualisation_PointClouds::Visualisation_PointClouds()
	:
	m_lightPos{ 0.0f,0.0f,0.0f },
	m_objectShader{ nullptr },
	m_lampShader{ nullptr },
	m_cubeColours{},
	m_cubePositions{},
	m_cube{},

	m_maxShapes{ 500 },
	m_shapesPerFrame{ 5 },
	m_shapeRes{ 5 },
	m_rescaleShapeOverTime{ false },
	m_shapeType{ false },
	m_colourDeviation{ 0.1f,0.2f,0.4f },
	m_distanceDeviation{ 1.0f }
{
}

void Visualisation_PointClouds::activate()
{
	m_objectShader = new Shader{ "../../assets/shaders/Cubes_Vertex.vs", "../../assets/shaders/Cubes_ObjectFragment.fs" };
	m_lampShader = new Shader{ "../../assets/shaders/Cubes_Vertex.vs", "../../assets/shaders/Cubes_LampFragment.fs" };

	MeshGenerator::generateCube(m_cube);
	/* gfxInit() isn't essential preparation as a user of Mesh, but it saves time on first draw. */
	m_cube.gfxInit();

	m_lightPos = { 0.0,0.0,-8.0 };
	m_active = true;
}

void Visualisation_PointClouds::deactivate()
{
	m_cube.gfxDelete();

	m_cubePositions.clear();
	m_cubeColours.clear();
	delete m_lampShader;
	m_lampShader = nullptr;
	delete m_objectShader;
	m_objectShader = nullptr;
	m_active = false;
}

void Visualisation_PointClouds::processSamples(const Buffer& buf, unsigned samples)
{
	MetaDistribution<double> dist{ 0.0f,0.01f,0.1f,0.01f };

	std::vector<std::vector<kiss_fft_cpx>> res = buf.fft();
	std::vector<std::vector<kiss_fft_cpx>> reduced;
	reduced.resize(res.size());
	for (int i = 0; i < res.size(); ++i)
	{
		// Take Middle
		size_t size = res[i].size();
		for (size_t j = (size / 4u); (float)j < ((float)size * (3./4.)); ++j)
			reduced[i].push_back(res[i][j]);
	}
	double maxIVal{ 0.0f };

	std::normal_distribution<double> metaDistX{ dist.createDistribution() };
	std::normal_distribution<double> metaDistY{ dist.createDistribution() };
	std::normal_distribution<double> metaDistZ{ dist.createDistribution() };
	AbstractRandomObject a{};
	double metaOffsetX{ metaDistX(a.mersenneTwister()) }, metaOffsetY{ metaDistY(a.mersenneTwister()) }, metaOffsetZ{ metaDistZ(a.mersenneTwister()) };

	std::map<int, std::deque<float>>& data{ buf.data() };
	std::deque<float>& ch{ data.at(0) };

	for (int i = 0; i < m_shapesPerFrame; ++i)
	{
		int fftIt = (reduced[0].size() * i) / m_shapesPerFrame;
		int posX = fftIt;
		int posY = (reduced[0].size() / 2) + (fftIt / 2);
		int posZ = (reduced[0].size() - 1) - fftIt;
		double meanX = reduced[0][posX].r;
		double meanY = reduced[0][posY].r;
		double meanZ = reduced[0][posZ].r;
		double rotateX = (i % 3 == 0) ? meanX : ((i % 3 == 1) ? meanY : meanZ);
		double rotateY = (i % 3 == 0) ? meanY : ((i % 3 == 1) ? meanZ : meanX);
		double rotateZ = (i % 3 == 0) ? meanZ : ((i % 3 == 1) ? meanX : meanY);

		double normalisation = std::max(std::max(std::abs(meanX), std::abs(meanY)), std::abs(meanZ));

		double colX{ 0.0 }, colY{ 0.0 }, colZ{ 0.0 };
		for (unsigned i = 0u; i < ch.size(); ++i)
		{
			if (i % 3 == 0)
				if (colX < ch[i])
					colX = ch[i];
			if (i % 7 == 0)
				if (std::abs(colY) < std::abs(ch[i]))
					colY = abs(ch[i]);
			if (i % 2 == 0)
				if (colZ < ch[i])
					colZ = ch[i];
		}
		/* Put a sin curve to increase intensity of lower values */
		//colX = sin((3.141 / 2.0) * colX * colY);
		//colY = sin((3.141 / 2.0) * colY * colZ);
		//colZ = sin((3.141 / 2.0) * colZ * colX);
		std::normal_distribution<double> xCol{ colX,m_colourDeviation.r };
		std::normal_distribution<double> yCol{ colY,m_colourDeviation.b };
		std::normal_distribution<double> zCol{ colZ,m_colourDeviation.g };
		glm::vec3 cubeColour{
			std::abs(xCol(a.mersenneTwister())),
			std::abs(yCol(a.mersenneTwister())),
			std::abs(zCol(a.mersenneTwister()))
		};

		std::normal_distribution<double> xPos{ rotateX, m_distanceDeviation.x * std::abs(metaOffsetX) };
		std::normal_distribution<double> yPos{ rotateY, m_distanceDeviation.y * std::abs(metaOffsetY) };
		std::normal_distribution<double> zPos{ rotateZ, m_distanceDeviation.z * std::abs(metaOffsetZ) };
		glm::vec3 cubePos{ xPos(a.mersenneTwister()),yPos(a.mersenneTwister()), zPos(a.mersenneTwister()) };


		glm::vec3 cubeScale{ 1.0f,1.0f,1.0f };


		m_cubePositions.insert(m_cubePositions.begin(), cubePos);
		m_cubeColours.insert(m_cubeColours.begin(), cubeColour);
		m_cubeScales.insert(m_cubeScales.begin(), cubeScale);
	}

	if (m_rescaleShapeOverTime)
		for (int i = 0; i < m_cubeScales.size(); ++i)
		{
			// At the beginning, scale is 0.0, then scaled to 1.0 1/4 of the way through,
				// then linearly scaled to zero.
			float quarterPos = (float)m_cubeScales.size() / 4.f;
			float finalPos = (float)m_cubeScales.size();
			if ((float)i <= quarterPos)
				m_cubeScales[i] = glm::vec3{ (float)i / (float)quarterPos };
			else
				m_cubeScales[i] = glm::vec3{ (1.0f - ((i - quarterPos) / (finalPos - quarterPos))) };
		}

	double signalPowerVal{ 0.0 };
	for (int i = 0; i < ch.size(); ++i)
		signalPowerVal += ((double)ch[i] * ch[i]);
	signalPowerVal /= (double)ch.size();

	m_signalPowerHistory.push_back(signalPowerVal);
	if (m_signalPowerHistory.size() > 10)
		m_signalPowerHistory.pop_front();

	double signalPowerAverage{ 0.0 };
	for (size_t i = 0; i < m_signalPowerHistory.size(); ++i)
		signalPowerAverage += m_signalPowerHistory[i];

	signalPowerAverage /= (double)m_signalPowerHistory.size();

	while (m_cubePositions.size() > m_maxShapes)
	{
		m_cubePositions.pop_back();
		m_cubeColours.pop_back();
		m_cubeScales.pop_back();
	}

	if (m_cubeColours.size() != m_cubePositions.size())
		m_cubeColours.resize(m_cubePositions.size(), glm::vec3{ 1.0f,0.0f,0.0f });
	if (m_cubeScales.size() != m_cubePositions.size())
		m_cubeScales.resize(m_cubePositions.size(), glm::vec3{ 1.0f,1.0f,1.0f });
}

void Visualisation_PointClouds::renderFrame(const Camera& camera, const Timestep& ts)
{
	double t{ ts.current().count() };


	m_objectShader->use();
	m_objectShader->setVec3("lightColour", glm::vec3{ 1.0f,0.5f,0.31f });
	m_objectShader->setVec3("objectColour", glm::vec3{ 1.0f,0.5f,0.31f });

	/* TODO: A scene should handle this! */
	glm::mat4 projection{ camera.projectionMatrix() };
	m_objectShader->setMat4("projection", projection);
	glm::mat4 view = camera.GetViewMatrix();
	m_objectShader->setMat4("view", view);

	glm::mat4 lightModel{ 1.0f };
	m_lightPos = glm::vec3(10 * sin(t), 10 * cos(t), 10 * sin(t));
	lightModel = glm::translate(lightModel, m_lightPos);
	lightModel = glm::scale(lightModel, glm::vec3{ 0.2f });
	m_objectShader->setVec3("lightPos", m_lightPos);
	m_objectShader->setVec3("viewPos", camera.m_position);

	for (unsigned int i = 0; i < m_cubePositions.size(); i++)
	{
		glm::mat4 model{ 1.0f };
		model = glm::translate(model, m_cubePositions[i]);
		model = glm::scale(model, m_cubeScales[i] * glm::vec3{ 0.05f,0.05f,0.05f });

		//*m_cubeColours[i].r * m_cubeColours[i].g * m_cubeColours[i].g
		float angle = 0.4f * (float)i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		m_objectShader->setMat4("model", model);

		m_objectShader->setVec3("objectColour", m_cubeColours[i]);
		m_cube.draw(m_objectShader);
	}
	m_lampShader->use();
	m_lampShader->setMat4("projection", projection);
	m_lampShader->setMat4("view", view);
	m_lampShader->setMat4("model", lightModel);
	m_cube.draw(m_objectShader);
}

void Visualisation_PointClouds::drawInterface()
{
	/* Possible settings: detail per object, shape type, number of shapes, number added per frame,
							colour deviation, distance deviation. */
							/* Another possiblity: Scale the shapes quickly as they appear, and scale them to zero as they
													approach the end of their existence*/

	enum ShapeType
	{
		Cube,
		Sphere,
		Torus,
		Arrow
	} shapeType = (ShapeType)m_shapeType;
	int resolution = m_shapeRes;
	ImGui::SliderInt("Max shapes", &m_maxShapes, 1, 20000);
	ImGui::SliderInt("Shapes per frame", &m_shapesPerFrame, 1, m_maxShapes);
	ImGui::Checkbox("Rescale", &m_rescaleShapeOverTime);
	ImGui::Combo("Shape", (int*)&shapeType, "Cube\0Sphere\0Torus\0Arrow\0");
	ImGui::SliderInt("Resolution", &resolution, 3, 30);
	ImGui::SliderFloat3("Colour Deviation", (float*)& m_colourDeviation, 0.0f, 1.0f);
	ImGui::SliderFloat3("Distance Deviation", (float*)& m_distanceDeviation, 0.0f, 10.0f);

	if ((int)shapeType != m_shapeType || resolution != m_shapeRes)
	{
		switch (shapeType)
		{
		case Cube:
			MeshGenerator::generateCube(m_cube);
			break;
		case Sphere:
			MeshGenerator::generateSphere(resolution, m_cube);
			break;
		case Torus:
			MeshGenerator::generateTorus(resolution,0.5,1.0,m_cube);
			break;
		case Arrow:
			MeshGenerator::generateArrow(resolution,m_cube);
			break;
		default:
			MeshGenerator::generateCube(m_cube);
			break;
		}
		m_shapeType = (int)shapeType;
		m_shapeRes = resolution;
	}



}
