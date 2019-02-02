#include "Visualisation_Oscilloscope.h"
#include "Shader.h"
#include "Recorder.h"
#include "Buffer.h"

#include <iostream>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <GLM\glm.hpp>

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
	m_shader{ nullptr }, m_window{ win }
{
	glfwGetCurrentContext();
	glfwMakeContextCurrent(m_window); /* guawi - missed this. */

	m_shader = new Shader{ "../Shaders/Oscilloscope_Vertex.vs","../Shaders/Oscilloscope_Fragment.fs" };
	glGenBuffers(1, &m_VBO); /* Buffer has been created and id has been given to the m_VBO int. */
	glGenVertexArrays(1, &m_VAO);
}

Visualisation_Oscilloscope::~Visualisation_Oscilloscope()
{
	glfwTerminate();
	delete m_shader;
}

void Visualisation_Oscilloscope::drawVisualisations(const Buffer & buf)
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

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)verticesLeft.size(), &verticesLeft[0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);
	m_shader->use();
	glDrawArrays(GL_POINTS, 0, (GLsizei)verticesLeft.size() - 1);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)verticesRight.size(), &verticesRight[0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	m_shader->use();
	glDrawArrays(GL_POINTS, 0, (GLsizei)verticesRight.size() - 1);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	float max = buf.amplitude_peak();
	float average = buf.amplitude_average();
	float min = buf.amplitude_minimum();


	std::vector<vertex> maxVerts = std::vector<vertex>{ vertex{ -1.0f,max,0.0f },vertex{ 1.0f,max,0.0f } };
	std::vector<vertex> averageVerts = std::vector<vertex>{ vertex{ -1.0f,average,0.0f },vertex{ 1.0f,average,0.0f } };
	std::vector<vertex> minVerts = std::vector<vertex>{ vertex{ -1.0f,min,0.0f },vertex{ 1.0f,min,0.0f } };
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)maxVerts.size(), &maxVerts[0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	m_shader->use();
	glDrawArrays(GL_LINES, 0, (GLsizei)maxVerts.size());
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)averageVerts.size(), &averageVerts[0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	m_shader->use();
	glDrawArrays(GL_LINES, 0, (GLsizei)averageVerts.size());
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)minVerts.size(), &minVerts[0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	m_shader->use();
	glDrawArrays(GL_LINES, 0, (GLsizei)minVerts.size());
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
