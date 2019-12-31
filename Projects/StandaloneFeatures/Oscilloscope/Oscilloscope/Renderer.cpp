#include "Shader.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "Renderer.h"
#include "Recorder.h"
#include "Buffer.h"

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

Renderer::Renderer() :
	m_shader{nullptr}
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_window = glfwCreateWindow(1280, 720, "AudioVisual", nullptr, nullptr);
	if (m_window == nullptr)
	{
		std::cout << "Failed to create window using GLFW." << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(m_window); /* guawi - missed this. */
	glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback); /* missed this */

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialise GLAD." << std::endl;
		glfwTerminate();
	}

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;


	m_shader = new Shader{ "../vertex.vs","../fragment.fs" };
	glGenBuffers(1, &m_VBO); /* Buffer has been created and id has been given to the m_VBO int. */
	glGenVertexArrays(1, &m_VAO);
}

Renderer::~Renderer()
{
	glfwTerminate();
	delete m_shader;
}

void Renderer::renderLoop(const Recorder & recorder)
{
	while (!glfwWindowShouldClose(m_window))
	{
		Buffer buf = recorder.getBuffer();

		std::vector<float> data = buf.data();
		std::vector<vertex> verticesLeft;
		std::vector<vertex> verticesRight;

		/* x from -1 to 1*/
		float increment = ((float)2 / (float)data.size());

		for (unsigned long i = 0; i < data.size(); i += 2)
		{
			float x = -1.0f + (increment * (float)i);
			verticesLeft.push_back(vertex{ x,data[i] + 0.2f,0.0f });
		}
		for (unsigned long i = 1; i < data.size(); i += 2)
		{
			float x = -1.0f + (increment * (float)i);
			verticesRight.push_back(vertex{ x,data[i] - 0.2f,0.0f });
		}

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)verticesLeft.size(), &verticesLeft[0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
		glEnableVertexAttribArray(0);
		m_shader->use();
		glDrawArrays(GL_POINTS, 0, (GLsizei)verticesLeft.size()-1);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)verticesRight.size(), &verticesRight[0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
		glEnableVertexAttribArray(0);
		m_shader->use();
		glDrawArrays(GL_POINTS, 0, (GLsizei)verticesRight.size()-1);
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







		processInput(m_window);
		glfwSwapBuffers(m_window);
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}
