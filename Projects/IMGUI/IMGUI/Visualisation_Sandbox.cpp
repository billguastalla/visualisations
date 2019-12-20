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

#include "stbimage/stb_image.h"

Visualisation_Sandbox::Visualisation_Sandbox()
	:
	m_lightPos{ 0.0f,0.0f,0.0f },
	m_objectShader{ nullptr },
	m_meshTop{}, m_meshBottom{}, m_lightMesh{}
{
	MeshGenerator::generateGraph(200, 200, m_meshTop);
	MeshGenerator::generateGraph(200, 200, m_meshBottom);
	MeshGenerator::generateCube(m_lightMesh);


	float angle = 90;
	m_mainModelMat = glm::mat4{ 1.0 };
	m_mainModelMat = glm::rotate(m_mainModelMat, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
	m_mainModelMat = glm::scale(m_mainModelMat, glm::vec3{ 100.0,100.0,30.0 });
	m_bottomModelMat = glm::translate(m_mainModelMat, glm::vec3{ 0.0,0.0,-0.15 });
	m_topModelMat = glm::translate(m_mainModelMat, glm::vec3{ 0.0, 0.0,0.15 });


	//unsigned int texture;
	//glGenTextures(1, &texture);
	//glBindTexture(GL_TEXTURE_2D, texture);
	//// set the texture wrapping/filtering options (on the currently bound texture object)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//// load and generate the texture
	//int width, height, nrChannels;
	//unsigned char* data = stbi_load("texture.jpg", &width, &height, &nrChannels, 0);
	//if (data)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//{
	//	std::cout << "Failed to load texture" << std::endl;
	//}
	//stbi_image_free(data);
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

void Visualisation_Sandbox::processSamples(const Buffer& buf, unsigned samples)
{

	if (m_frameCounter % 10 == 0)
	{
		int size = std::floor(std::sqrt((float)buf.maxChannelFrameCount()));
		size--;

		std::deque<float> lc = buf.data(0);
		std::deque<float> rc = buf.data(1);
		// TODO: Interpolate incoming data, so you can see peaks move smoothly across surface.
		MeshGenerator::generateGraph(size, size, m_meshBottom, lc);
		MeshGenerator::generateGraph(size, size, m_meshTop, rc);

	}


	//m_bottomModelMat = glm::translate(m_mainModelMat, glm::vec3{ 0.0,0.0,-0.15 });
	//m_topModelMat = glm::translate(m_mainModelMat, glm::vec3{ 0.0, 0.0,0.15 });
	//m_topModelMat = glm::scale(m_topModelMat, glm::vec3{ 1.0,1.0,1.0 + pk });
	//m_bottomModelMat = glm::scale(m_bottomModelMat, glm::vec3{ 1.0,1.0,1.0 + min });
}

void Visualisation_Sandbox::renderFrame()
{
	++m_frameCounter;
	if (m_frameCounter % 100 == 0)
	{
		//MeshGenerator::generateGraph(80, 200, m_meshBottom);
		//MeshGenerator::generateGraph(200, 80, m_meshTop);
	}

	// activate shader
	m_objectShader->use();
	m_objectShader->setVec3("lightColour", glm::vec3{ 0.9f,0.8f,0.81f });

	// pass projection matrix to shader (note that in this case it could change every frame)
	glm::mat4 projection = glm::perspective(glm::radians(m_camera.m_zoom), (float)1920 / (float)1080, 0.1f, 100.0f);

	// camera/view transformation
	glm::mat4 view = m_camera.GetViewMatrix();

	m_objectShader->setMat4("projection", projection);
	m_objectShader->setMat4("view", view);
	m_objectShader->setMat4("model", m_mainModelMat);


	glm::mat4 lightModel{ 1.0f };
	m_lightPos = glm::vec3(30 * cos(0.4 * glfwGetTime()), 0, 30 * sin(0.4 * glfwGetTime()));
	lightModel = glm::translate(lightModel, m_lightPos);
	lightModel = glm::scale(lightModel, glm::vec3{ 0.2f });
	m_objectShader->setVec3("lightPos", m_lightPos);
	m_objectShader->setVec3("viewPos", m_camera.m_position);


	m_objectShader->setMat4("model", m_bottomModelMat);
	m_objectShader->setVec3("objectColour", glm::vec3{ 0.5f,0.2f,0.11f });
	m_meshBottom.draw(m_objectShader);
	m_objectShader->setMat4("model", m_topModelMat);
	m_objectShader->setVec3("objectColour", glm::vec3{ 0.2f,0.3f,0.61f });
	m_meshTop.draw(m_objectShader);


	m_objectShader->setMat4("model", lightModel);
	m_objectShader->setVec3("objectColour", glm::vec3{ 0.8f,0.6f,0.6f });
	m_lightMesh.draw(m_objectShader);
}
