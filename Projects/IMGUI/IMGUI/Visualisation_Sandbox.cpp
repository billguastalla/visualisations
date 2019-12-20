#include "Visualisation_Sandbox.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Buffer.h"

#include "MetaDistribution.h"

#include <iostream>
#include <complex>
#include <vector>

#include "MeshGenerator.h"

Visualisation_Sandbox::Visualisation_Sandbox()
	:
	m_lightPos{ 0.0f,0.0f,0.0f },
	m_objectShader{ nullptr },
	m_meshTop{}, m_meshBottom{}, m_lightMesh{}
{
	MeshGenerator::generateGraph(200, 200, m_meshTop);
	MeshGenerator::generateGraph(200, 200, m_meshBottom);
	MeshGenerator::generateCube(m_lightMesh);
}

void Visualisation_Sandbox::activate()
{
	m_objectShader = new Shader{ "../Shaders/Mesh_Vertex.vs", "../Shaders/Mesh_Fragment.fs" };
	m_lightPos = { 0.0,0.0,-8.0 };
}

void Visualisation_Sandbox::deactivate()
{
	delete m_objectShader;
	m_objectShader = nullptr;
}

void Visualisation_Sandbox::processSamples(const Buffer & buf, unsigned samples)
{
	m_camera.m_position.x = 3 * sin(glfwGetTime());
	m_camera.m_position.z = 3 * cos(glfwGetTime());
	double yawRadians = atan2(m_camera.m_position.x, m_camera.m_position.z);
	double yawDegrees = glm::degrees(yawRadians);

	glm::vec3 front;
	front.x = -m_camera.m_position.x;
	front.y = -m_camera.m_position.y;
	front.z = -m_camera.m_position.z;
	m_camera.m_front = glm::normalize(front);
}

void Visualisation_Sandbox::renderFrame()
{
	// activate shader
	m_objectShader->use();
	m_objectShader->setVec3("lightColour", glm::vec3{ 0.9f,0.8f,0.81f });

	// pass projection matrix to shader (note that in this case it could change every frame)
	glm::mat4 projection = glm::perspective(glm::radians(m_camera.m_zoom), (float)1920 / (float)1080, 0.1f, 100.0f);

	// camera/view transformation
	glm::mat4 view = m_camera.GetViewMatrix();

	glm::mat4 model{ 1.0f };
	float angle = 90;
	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3{ 100.0,100.0,30.0 });

	m_objectShader->setMat4("projection", projection);
	m_objectShader->setMat4("view", view);
	m_objectShader->setMat4("model", model);


	glm::mat4 lightModel{ 1.0f };
	m_lightPos = glm::vec3(30 * cos(0.2*glfwGetTime()),0, 30 * sin(0.2*glfwGetTime()));
	lightModel = glm::translate(lightModel, m_lightPos);
	lightModel = glm::scale(lightModel, glm::vec3{ 0.2f });
	m_objectShader->setVec3("lightPos", m_lightPos);
	m_objectShader->setVec3("viewPos", m_camera.m_position);


	glm::mat4 bottomMeshModelMatrix = glm::translate(model, glm::vec3{ 0.0,0.0,-0.15 });
	m_objectShader->setMat4("model", bottomMeshModelMatrix);
	m_objectShader->setVec3("objectColour", glm::vec3{ 0.5f,0.2f,0.11f });
	m_meshBottom.draw(m_objectShader);
	glm::mat4 topMeshModelMatrix = glm::translate(model, glm::vec3{ 0.0, 0.0,0.15 });
	m_objectShader->setMat4("model", topMeshModelMatrix);
	m_objectShader->setVec3("objectColour", glm::vec3{ 0.2f,0.3f,0.61f });
	m_meshTop.draw(m_objectShader);


	m_objectShader->setMat4("model", lightModel);
	m_objectShader->setVec3("objectColour", glm::vec3{ 0.8f,0.6f,0.6f });
	m_lightMesh.draw(m_objectShader);
}
