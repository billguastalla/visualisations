#include "Visualisation_GameOfLife.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Buffer.h"

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
	m_frameLoop{ 0 }
{
	std::vector<int> dims;
	dims.push_back(10);
	dims.push_back(10);
	dims.push_back(10);
	m_game.reset(new Game{ dims });
	m_game->setRule(4, std::make_pair(true, false));
	m_game->currentGrid()->addCell(1);

}

void Visualisation_GameOfLife::activate()
{
	m_objectShader = new Shader{ "../Shaders/Cubes_Vertex.vs", "../Shaders/Cubes_ObjectFragment.fs" };
	m_lampShader = new Shader{ "../Shaders/Cubes_Vertex.vs", "../Shaders/Cubes_LampFragment.fs" };

	m_lightPos = { 0.0,0.0,-8.0 };

	m_vertices = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	// world space positions of our cubes
	m_cubePositions = {glm::vec3(0.0f,0.0f,0.0f)};
	m_cubeColours = {glm::vec3(0.0f,0.0f,0.0f)};

	glGenVertexArrays(1, &m_cubeVAO);
	glGenBuffers(1, &m_cubeVBO);
	glBindVertexArray(m_cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &m_lightVAO);
	glBindVertexArray(m_lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void Visualisation_GameOfLife::deactivate()
{
	glDeleteVertexArrays(1, &m_cubeVAO);
	glDeleteVertexArrays(1, &m_lightVAO);
	glDeleteBuffers(1, &m_cubeVBO);

	m_cubePositions.clear();
	m_vertices.clear();

	delete m_lampShader;
	m_lampShader = nullptr;
	delete m_objectShader;
	m_objectShader = nullptr;
}

void Visualisation_GameOfLife::processSamples(const Buffer & buf, unsigned samples)
{
	MetaDistribution<double> randColour{0.5f,0.1f,0.001f,0.3f };
	std::normal_distribution<double> colImplX{ randColour.createDistribution() };
	std::normal_distribution<double> colImplY{ randColour.createDistribution() };
	std::normal_distribution<double> colImplZ{ randColour.createDistribution() };
	AbstractRandomObject a;

	if (m_frameLoop % 4 == 0)
	{
		Grid * grid = m_game->currentGrid();
		/* Get indexes or floats for position:
		-> Probably should change grid to return list of coords of alive things.
		Would have to be a vector of vectors unfortunately because of dynamically
		typed dimension.
		*/

		m_cubePositions.clear();
		m_cubeColours.clear();

		std::vector<int> dims{ grid->dimensions() };
		std::vector<std::vector<int>> coords{ grid->aliveCoords() };
		for (auto i = coords.begin(); i != coords.end(); ++i)
		{
			glm::vec3 t_position{ (2.0 * (float)(*i)[0] / (float)dims[0]) - 1.0,
				(2.0 * (float)(*i)[1] / (float)dims[1]) - 1.0,
				(2.0 * (float)(*i)[2] / (float)dims[2]) - 1.0 };
			m_cubePositions.push_back(t_position);
			m_cubeColours.push_back(glm::vec3{ std::abs(colImplX(a.randDevice())),std::abs(colImplY(a.randDevice())),std::abs(colImplZ(a.randDevice()))});
		}

		if (m_coordsHistory.size() != 0)
			for (auto i = m_coordsHistory.begin(); i != m_coordsHistory.end(); ++i)
				if (*i == coords)
				{
					m_coordsHistory.clear();
					m_game->randomRuleset();

					m_game->currentGrid()->clear();
					m_game->currentGrid()->insertPrimitive();

					//m_game->currentGrid()->setRandAlive(30);
					break;
				}
		m_coordsHistory.push_back(coords);

		if (coords.size() == 0 || m_coordsHistory.size() > 100)
		{
			m_coordsHistory.clear();
			m_game->randomRuleset();

			m_game->currentGrid()->clear();
			m_game->currentGrid()->insertPrimitive();
		}
		m_game->nextTurn();
		m_frameLoop = 0;
	}
	m_frameLoop++;


	m_camera.m_position.x = 4.5 * sin(glfwGetTime());
	m_camera.m_position.z = 4.5 * cos(glfwGetTime());
	double yawRadians = atan2(m_camera.m_position.x, m_camera.m_position.z);
	double yawDegrees = glm::degrees(yawRadians);

	glm::vec3 front;
	front.x = -m_camera.m_position.x;
	front.y = -m_camera.m_position.y;
	front.z = -m_camera.m_position.z;
	m_camera.m_front = glm::normalize(front);
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
	m_lightPos = glm::vec3(10 * sin(glfwGetTime()), 10.0, 10.0);
	lightModel = glm::translate(lightModel, m_lightPos);
	lightModel = glm::scale(lightModel, glm::vec3{ 0.2f });
	m_objectShader->setVec3("lightPos", m_lightPos);
	m_objectShader->setVec3("viewPos", m_camera.m_position);

	// render boxes
	glBindVertexArray(m_cubeVAO);
	for (unsigned int i = 0; i < m_cubePositions.size(); i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model{ 1.0f };
		model = glm::translate(model, m_cubePositions[i]);
		model = glm::scale(model, glm::vec3{ 0.1f,0.1f,0.1f });

		//*m_cubeColours[i].r * m_cubeColours[i].g * m_cubeColours[i].g
		float angle = 0.4f * (float)i;
		//model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		m_objectShader->setMat4("model", model);

		m_objectShader->setVec3("objectColour", m_cubeColours[i]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	m_lampShader->use();
	m_lampShader->setMat4("projection", projection);
	m_lampShader->setMat4("view", view);
	m_lampShader->setMat4("model", lightModel);
	glBindVertexArray(m_lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Visualisation_GameOfLife::drawInterface()
{

}
