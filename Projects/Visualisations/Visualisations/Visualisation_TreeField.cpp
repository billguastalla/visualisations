#include "Visualisation_TreeField.h"

#include "Shader.h"
#include "Buffer.h"
#include "MetaDistribution.h"

#include <imgui/imgui.h>
#include <GLFW\glfw3.h>
#include <complex>
#include <vector>

Visualisation_TreeField::Visualisation_TreeField()
	:
	m_lightPos{ 0.0f,0.0f,0.0f },
	m_objectShader{ nullptr },
	m_lampShader{ nullptr },

	m_field{}
{
}

void Visualisation_TreeField::activate()
{
	m_objectShader = new Shader{ "../Shaders/Cubes_Vertex.vs", "../Shaders/Cubes_ObjectFragment.fs" };
	m_lampShader = new Shader{ "../Shaders/Cubes_Vertex.vs", "../Shaders/Cubes_LampFragment.fs" };

	MeshGenerator::generateSphere(40, m_lamp);
	m_field.setup();

	m_lightPos = { 0.0,0.0,-8.0 };
	m_active = true;
}

void Visualisation_TreeField::deactivate()
{
	
	delete m_lampShader;
	m_lampShader = nullptr;
	delete m_objectShader;
	m_objectShader = nullptr;
	m_active = false;
}

void Visualisation_TreeField::processSamples(const Buffer& buf, unsigned samples)
{
}

void Visualisation_TreeField::renderFrame(const Camera& camera, const Timestep& ts)
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


	m_field.draw(camera.m_position);

	m_lampShader->use();
	m_lampShader->setMat4("projection", projection);
	m_lampShader->setMat4("view", view);
	m_lampShader->setMat4("model", lightModel);
	m_lamp.draw(m_objectShader);
}

void Visualisation_TreeField::drawInterface()
{

}
