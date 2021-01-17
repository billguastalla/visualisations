#include "Visualisation_Cubes.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../graphics/Shader.h"
#include "../audio/Buffer.h"

#include <iostream>
#include <complex>
#include <vector>

Visualisation_Cubes::Visualisation_Cubes()
	:
	m_lightPos{0.0f,0.0f,0.0f},
	m_objectShader{ nullptr },
	m_lampShader{ nullptr },
	m_cubeVAO{},
	m_cubeVBO{},
	m_lightVAO{},
	m_cubePositions{}
{
}

void Visualisation_Cubes::activate()
{
	m_objectShader = new Shader{ "../../assets/shaders/Cubes_Vertex.vs", "../../assets/shaders/Cubes_ObjectFragment.fs" };
	m_lampShader = new Shader{ "../../assets/shaders/Cubes_Vertex.vs", "../../assets/shaders/Cubes_LampFragment.fs" };

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
	m_cubePositions = {
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

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

	m_active = true;
}

void Visualisation_Cubes::deactivate()
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

	m_active = false;
}

void Visualisation_Cubes::processSamples(const Buffer & buf, unsigned samples)
{
}

void Visualisation_Cubes::renderFrame(const Camera& camera, const Timestep& ts)
{
	double t{ts.current().count()};

	// activate shader
	m_objectShader->use();
	m_objectShader->setVec3("lightColour", glm::vec3{ 1.0f,0.5f,0.31f });
	m_objectShader->setVec3("objectColour", glm::vec3{ 1.0f,0.5f,0.31f });

	// pass projection matrix to shader (note that in this case it could change every frame)
	glm::mat4 projection{ camera.projectionMatrix() };
	m_objectShader->setMat4("projection", projection);

	// camera/view transformation
	glm::mat4 view = camera.GetViewMatrix();
	m_objectShader->setMat4("view", view);

	glm::mat4 lightModel{ 1.0f };
	m_lightPos = glm::vec3(5 * sin(0.1 * t), 10 * cos(t), 10 * sin(t));
	lightModel = glm::translate(lightModel, m_lightPos);
	lightModel = glm::scale(lightModel, glm::vec3{ 0.2f });
	m_objectShader->setVec3("lightPos", m_lightPos);
	m_objectShader->setVec3("viewPos", camera.m_position);
		   
	// render boxes
	glBindVertexArray(m_cubeVAO);
	for (unsigned int i = 0; i < 9; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model{ 1.0f };
		model = glm::translate(model, m_cubePositions[i]);
		float angle = i * 20.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		m_objectShader->setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	m_lampShader->use();
	m_lampShader->setMat4("projection", projection);
	m_lampShader->setMat4("view", view);
	m_lampShader->setMat4("model", lightModel);
	glBindVertexArray(m_lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

