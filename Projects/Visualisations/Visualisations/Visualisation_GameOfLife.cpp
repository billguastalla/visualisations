#include "Visualisation_GameOfLife.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "MeshGenerator.h"
#include "Buffer.h"

#include <imgui/imgui.h>
#include "MetaDistribution.h"

#include "Visualisation_GOL\GameOfLife_Game.h"

#include <iostream>
#include <complex>
#include <vector>

Visualisation_GameOfLife::Visualisation_GameOfLife()
	:
	m_lightPos{ 0.0f,0.0f,0.0f },

	m_objectShader{ nullptr },
	m_lampShader{ nullptr },
	m_game{ nullptr },
	m_frameLoop{ 0 },

	m_frameUpdateRate{ 15 },
	m_reinitExtent{ 5 }
{
}

void Visualisation_GameOfLife::activate()
{
	std::vector<int> dims;
	dims.push_back(20);
	dims.push_back(20);
	dims.push_back(20);
	m_game.reset(new Game{ dims });
	m_game->setRule(4, std::make_pair(true, false));
	m_game->currentGrid().addCell(1);

	m_objectShader = new Shader{ "../Shaders/Cubes_Vertex.vs", "../Shaders/Cubes_ObjectFragment.fs" };
	m_lampShader = new Shader{ "../Shaders/Cubes_Vertex.vs", "../Shaders/Cubes_LampFragment.fs" };

	m_lightPos = { 0.0,0.0,-8.0 };

	MeshGenerator::generateCube(m_cube);

	// world space positions of our cubes
	m_cubePositions = { glm::vec3(0.0f,0.0f,0.0f) };
	m_cubeColours = { glm::vec3(0.0f,0.0f,0.0f) };

	m_active = true;
}

void Visualisation_GameOfLife::deactivate()
{
	delete m_game.release();



	m_cube.clear();

	m_cubePositions.clear();
	m_vertices.clear();

	delete m_lampShader;
	m_lampShader = nullptr;
	delete m_objectShader;
	m_objectShader = nullptr;
}

void Visualisation_GameOfLife::processSamples(const Buffer& buf, unsigned samples)
{
	MetaDistribution<double> randColour{ 0.5f,0.1f,0.001f,0.3f };
	std::normal_distribution<double> colImplX{ randColour.createDistribution() };
	std::normal_distribution<double> colImplY{ randColour.createDistribution() };
	std::normal_distribution<double> colImplZ{ randColour.createDistribution() };
	AbstractRandomObject a;


	m_frameLoop++;
	if (!m_game->dimensions().empty() && m_frameLoop % m_frameUpdateRate == 0)
	{
		const Grid & grid = m_game->currentGrid();
		/* Get indexes or floats for position:
		-> Probably should change grid to return list of coords of alive things.
		Would have to be a vector of vectors unfortunately because of dynamically
		typed dimension.
		*/

		m_cubePositions.clear();
		m_cubeColours.clear();

		std::vector<int> dims{ grid.dimensions() };
		std::vector<std::vector<int>> coords{ grid.aliveCoords() };
		for (auto i = coords.begin(); i != coords.end(); ++i)
		{
			glm::vec3 t_position
			{
				(float)((*i)[0]) * 0.01,
				(float)((*i)[1]) * 0.01,
				(float)((*i)[2]) * 0.01
			};
			m_cubePositions.push_back(t_position);

			glm::vec3 colour{0.0,0.0,0.0};

			ImGui::ColorConvertHSVtoRGB((float)((*i)[0]) / dims[0],(float)((*i)[1]) / dims[1],(float)((*i)[2]) / dims[2],
				colour.r,
				colour.g,
				colour.b);

			m_cubeColours.push_back(colour);
			//m_cubeColours.push_back(glm::vec3{ std::abs(colImplX(a.randDevice())),std::abs(colImplY(a.randDevice())),std::abs(colImplZ(a.randDevice())) });
		}
		m_game->nextTurn();
	}

	//	if (m_coordsHistory.size() != 0)
	//		for (auto i = m_coordsHistory.begin(); i != m_coordsHistory.end(); ++i)
	//			if (*i == coords)
	//			{
	//				m_coordsHistory.clear();
	//				m_game->randomRuleset();

	//				m_game->currentGrid()->clear();
	//				m_game->currentGrid()->insertPrimitive();

	//				//m_game->currentGrid()->setRandAlive(30);
	//				break;
	//			}
	//	m_coordsHistory.push_back(coords);

	//	if (coords.size() == 0 || m_coordsHistory.size() > 100)
	//	{
	//		m_coordsHistory.clear();
	//		m_game->randomRuleset();

	//		m_game->currentGrid()->clear();
	//		m_game->currentGrid()->insertPrimitive();
	//	}
	//	m_game->nextTurn();
	//	m_frameLoop = 0;
	//}
	//m_frameLoop++;


	//m_camera.m_position.x = 4.5 * sin(glfwGetTime());
	//m_camera.m_position.z = 4.5 * cos(glfwGetTime());
	//double yawRadians = atan2(m_camera.m_position.x, m_camera.m_position.z);
	//double yawDegrees = glm::degrees(yawRadians);

	//glm::vec3 front;
	//front.x = -m_camera.m_position.x;
	//front.y = -m_camera.m_position.y;
	//front.z = -m_camera.m_position.z;
	//m_camera.m_front = glm::normalize(front);
}

void Visualisation_GameOfLife::renderFrame()
{
	float currentFrame = glfwGetTime();
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;

	// activate shader
	m_objectShader->use();
	m_objectShader->setVec3("lightColour", glm::vec3{ 1.0f,0.5f,0.31f });
	m_objectShader->setVec3("objectColour", glm::vec3{ 1.0f,0.5f,0.31f });

	// pass projection matrix to shader (note that in this case it could change every frame)
	glm::mat4 projection = glm::perspective(glm::radians(m_camera.m_zoom), (float)1920 / (float)1080, 0.1f, 100.0f);
	m_objectShader->setMat4("projection", projection);

	// camera/view transformation
	glm::mat4 view = m_camera.GetViewMatrix();

	m_objectShader->setMat4("view", view);

	glm::mat4 lightModel{ 1.0f };

	glm::vec3 upMod = m_camera.m_up;
	upMod *= sin((float)m_frameLoop / 600.0);
	glm::vec3 rightMod = m_camera.m_up;
	upMod *= cos((float)m_frameLoop / 600.0);

	m_lightPos = m_camera.m_position + upMod + rightMod;

	lightModel = glm::translate(lightModel, m_lightPos);
	lightModel = glm::scale(lightModel, glm::vec3{ 0.2f });
	m_objectShader->setVec3("lightPos", m_lightPos);
	m_objectShader->setVec3("viewPos", m_camera.m_position);

	// render boxes
	for (unsigned int i = 0; i < m_cubePositions.size(); i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model{ 1.0f };
		model = glm::translate(model, m_cubePositions[i]);
		model = glm::scale(model, glm::vec3{ 0.01f,0.01f,0.01f });

		m_objectShader->setMat4("model", model);
		m_objectShader->setVec3("objectColour", m_cubeColours[i]);
		m_cube.draw(m_objectShader);
	}
	m_lampShader->use();
	m_lampShader->setMat4("projection", projection);
	m_lampShader->setMat4("view", view);
	m_lampShader->setMat4("model", lightModel);
	m_cube.draw(m_lampShader);
}

void Visualisation_GameOfLife::drawInterface()
{
	std::vector<int> dims{ m_game->currentGrid().dimensions() };
	if (dims.size() == 3)
	{
		ImGui::SliderInt3("Dimensions", (int*)&dims[0], 2, 500);
		if (dims != m_game->currentGrid().dimensions())
			m_game->setDimensions(dims);
	}

	ImGui::SliderFloat("Spacing", &m_spacing, 0.0f, 10.0f);

	bool reinit = ImGui::Button("Re-initialise");
	ImGui::SameLine();
	ImGui::SliderInt("Extent", &m_reinitExtent, 1, 10);
	ImGui::SliderInt("Update Every # Frames", &m_frameUpdateRate, 1, 60);


	if (reinit)
	{
		m_game->currentGrid().clear();
		m_game->currentGrid().insertPrimitive(m_reinitExtent);
	}

	bool randomRules = ImGui::Button("Random Ruleset");
	if (randomRules)
		m_game->randomRuleset();

	
	



	std::vector<std::pair<bool, bool>> & rules{ m_game->getRules() };
	ImGui::Text("Rules");
	for (size_t i = 0; i < rules.size(); ++i)
	{
		std::string id = std::to_string(i).c_str();
		bool d = ImGui::Checkbox(std::string{ id + "D" }.c_str(), &rules[i].first);
		ImGui::SameLine();
		bool a = ImGui::Checkbox(std::string{ id + "A" }.c_str(), &rules[i].second);

		if (d || a)
			m_game->setRule(i, std::pair<bool, bool>{rules[i].first, rules[i].second});
	}




}
