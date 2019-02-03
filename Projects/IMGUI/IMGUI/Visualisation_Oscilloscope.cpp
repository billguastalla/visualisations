#include "Visualisation_Oscilloscope.h"
#include "Vertex.h"
#include "Shader.h"
#include "Recorder.h"
#include "Buffer.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <GLM\glm.hpp>
#include <iostream>

void framebufferSizeCallback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

Visualisation_Oscilloscope::Visualisation_Oscilloscope(GLFWwindow * win) :
	m_shader{ nullptr }, m_window{ win },
	m_leftScopeElemCount{0},
	m_rightScopeElemCount{0},
	m_maxLineElemCount{ 0 },
	m_averageLineElemCount{0},
	m_minLineElemCount{ 0 }
{
}

Visualisation_Oscilloscope::~Visualisation_Oscilloscope()
{
}

void Visualisation_Oscilloscope::processSamples(const Buffer & buf, unsigned samples)
{
	std::deque<float> leftData = buf.data(0);
	std::deque<float> rightData = buf.data(1);
	std::vector<vertex> verticesLeft;
	std::vector<vertex> verticesRight;
	/* x from -1 to 1*/
	float increment = ((float)2 / (float)leftData.size());

	for (int i = 0; i < leftData.size(); ++i)
	{
		float x = -1.0f + (increment * (float)i);
		verticesLeft.push_back(vertex{ x,leftData[i] + 0.2f,0.0f });
	}
	for (int i = 0; i < rightData.size(); ++i)
	{
		float x = -1.0f + (increment * (float)i) + increment;
		verticesRight.push_back(vertex{ x,rightData[i] - 0.2f,0.0f });
	}
	float max = buf.amplitude_peak();
	float average = buf.amplitude_average();
	float min = buf.amplitude_minimum();
	std::vector<vertex> maxVerts = std::vector<vertex>{ vertex{ -1.0f,max,0.0f },vertex{ 1.0f,max,0.0f } };
	std::vector<vertex> averageVerts = std::vector<vertex>{ vertex{ -1.0f,average,0.0f },vertex{ 1.0f,average,0.0f } };
	std::vector<vertex> minVerts = std::vector<vertex>{ vertex{ -1.0f,min,0.0f },vertex{ 1.0f,min,0.0f } };

	m_leftScopeElemCount = verticesLeft.size();
	m_rightScopeElemCount = verticesRight.size();
	m_maxLineElemCount = maxVerts.size();
	m_averageLineElemCount = averageVerts.size();
	m_minLineElemCount = minVerts.size();

	glBindVertexArray(m_leftScopeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_leftScopeVBO); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)verticesLeft.size(), &verticesLeft[0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(m_rightScopeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_rightScopeVBO); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)verticesRight.size(), &verticesRight[0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(m_maxLineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_maxLineVBO); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)maxVerts.size(), &maxVerts[0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(m_averageLineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_averageLineVBO); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)averageVerts.size(), &averageVerts[0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(m_minLineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_minLineVBO); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)minVerts.size(), &minVerts[0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
}

void Visualisation_Oscilloscope::renderFrame()
{
	m_shader->use();

	glBindBuffer(GL_ARRAY_BUFFER, m_leftScopeVBO);
	glBindVertexArray(m_leftScopeVAO);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_POINTS, 0, m_leftScopeElemCount - 1);

	glBindBuffer(GL_ARRAY_BUFFER, m_rightScopeVBO);
	glBindVertexArray(m_rightScopeVAO);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_POINTS, 0, m_rightScopeElemCount - 1);


	glBindBuffer(GL_ARRAY_BUFFER, m_minLineVBO);
	glBindVertexArray(m_minLineVAO);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, m_minLineElemCount);

	glBindBuffer(GL_ARRAY_BUFFER, m_averageLineVBO);
	glBindVertexArray(m_averageLineVAO);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, m_averageLineElemCount);

	glBindBuffer(GL_ARRAY_BUFFER, m_maxLineVBO);
	glBindVertexArray(m_maxLineVAO);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, m_maxLineElemCount);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Visualisation_Oscilloscope::activate()
{
	m_shader = new Shader{ "../Shaders/Oscilloscope_Vertex.vs","../Shaders/Oscilloscope_Fragment.fs" };

	glGenBuffers(1, &m_leftScopeVBO);
	glGenBuffers(1, &m_rightScopeVBO);
	glGenBuffers(1, &m_minLineVBO);
	glGenBuffers(1, &m_averageLineVBO);
	glGenBuffers(1, &m_maxLineVBO);

	glGenVertexArrays(1, &m_leftScopeVAO);
	glGenVertexArrays(1, &m_rightScopeVAO);
	glGenVertexArrays(1, &m_minLineVAO);
	glGenVertexArrays(1, &m_averageLineVAO);
	glGenVertexArrays(1, &m_maxLineVAO);

	m_active = true;
}

void Visualisation_Oscilloscope::deactivate()
{
	glDeleteBuffers(1, &m_leftScopeVBO);
	glDeleteBuffers(1, &m_rightScopeVBO);
	glDeleteBuffers(1, &m_minLineVBO);
	glDeleteBuffers(1, &m_averageLineVBO);
	glDeleteBuffers(1, &m_maxLineVBO);

	glDeleteVertexArrays(1, &m_leftScopeVAO);
	glDeleteVertexArrays(1, &m_rightScopeVAO);
	glDeleteVertexArrays(1, &m_minLineVAO);
	glDeleteVertexArrays(1, &m_averageLineVAO);
	glDeleteVertexArrays(1, &m_maxLineVAO);

	delete m_shader;
	m_shader = nullptr;

	m_active = false;
}
