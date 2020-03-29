#include "Visualisation_Oscilloscope.h"
#include "Vertex.h"
#include "Shader.h"
#include "Recorder.h"
#include "Buffer.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <GLM\glm.hpp>
#include <iostream>

//void framebufferSizeCallback(GLFWwindow * window, int width, int height)
//{
//	glViewport(0, 0, width, height);
//}
//
//void processInput(GLFWwindow * window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//}

Visualisation_Oscilloscope::Visualisation_Oscilloscope() : Visualisation{},
m_timeShader{ nullptr }, m_freqShader{ nullptr },
m_audioChannelCount{ 2 },
m_frequencyElemCounts{}
{
	m_frequencyVAO.resize(m_audioChannelCount, 0u);
	m_frequencyVBO.resize(m_audioChannelCount, 0u);
	m_timeVAO.resize(m_audioChannelCount, 0u);
	m_timeVBO.resize(m_audioChannelCount, 0u);
	m_timeElemCounts.resize(m_audioChannelCount, 0u);
	m_frequencyElemCounts.resize(m_audioChannelCount, 0u);
}

Visualisation_Oscilloscope::~Visualisation_Oscilloscope()
{
}

void Visualisation_Oscilloscope::processSamples(const Buffer & buf, unsigned samples)
{
	/* Refactor independent of channels, so that changing channel count does not break visualisation.. */
	size_t chCount{ buf.channelCount() };
	if (chCount == 0)
		return;

	m_audioChannelCount = chCount;

	std::map<int, std::deque<float>> timeInputData = buf.data();
	std::vector<std::vector<kiss_fft_cpx>> freqInputData{ buf.fft() };

	/* TODO: Caching for buffer analysis; needs to be migrated to another object & held by reference with trigger for data changed */
	/* This data, now normalised, has both real and imaginary magnitudes clamped between -1.0f & 1.0f */
	Buffer::normaliseFFT(freqInputData);

	/* SHOULD BE CONTIGUOUS.. */
	std::vector<std::vector<vertex>> verticesTimeDomain;
	std::vector<std::vector<vertex>> verticesFrequencyDomain;
	verticesTimeDomain.resize(chCount);
	verticesFrequencyDomain.resize(chCount);

	/* Offsets/Multiplier in X, Y:
			-> Time/Freq has multiplier = 0.5, offset =
			-> Channel has: multiplier = 1/chCount , offset =
	*/
	float xChannelMultiplier{ (float)1.0f / chCount };

	float xChannelOffset{ 0.0f };
	float yDataTypeOffset{ 0.0f };
	float yDataTypeMultiplier{ 0.5f };

	for (size_t c = 0; c < chCount; ++c)
	{
		xChannelOffset = (((float)c * xChannelMultiplier) * 2.0f) - 1.0f;
		xChannelOffset += (0.6f / (2.0f));

		size_t s{ 0u };
		float sOffset{ 0.0f };
		/* Time-domain Vertices: */
		for (std::deque<float>::iterator i = timeInputData[c].begin(); i != timeInputData[c].end(); ++i)
		{
			/* */
			sOffset = ((float)s / (float)timeInputData.at(c).size());
			sOffset *= 0.6f;

			verticesTimeDomain[c].push_back(vertex{((sOffset + xChannelOffset)),((*i * 0.5f) + 0.5f) ,0.0f });
			++s;
		}
		s = 0;
		/* Freq-domain Vertices: */
		for (std::vector<kiss_fft_cpx>::iterator i = freqInputData[c].begin(); i != freqInputData[c].end(); ++i)
		{
			sOffset = ((float)s / (float)freqInputData[c].size());
			sOffset *= 0.6f;

			/* Position Vertex (Magnitude) */
			verticesFrequencyDomain[c].push_back(vertex{(sOffset + xChannelOffset),((i->r * 0.5f) - 0.5f),0.0f });

			/* Colour Vertex (Phase) */
			verticesFrequencyDomain[c].push_back(vertex{ (abs(i->i * 0.5f) + 0.5f),0.1f,0.8f });
			++s;
		}
		s = 0;
	}



	/*
		-> Four Graphs: Top half is Time-domain, Bottom-half is Freq-domain, Left and Right are channels
		-> Vertices for top are bound between y = 0.0f & y = 1.0f
		-> Vertices for right are bound between x = 0.0f & x = 1.0f

		-> Time-domain data has one vertex attribute for position
		-> Frequency-domain data has two vertex attributes: one for position, one for colour.
			-> Hue of frequency domain data is determined by complex plane.

		-> All FFT data needs to be normalised.
	*/

	//std::vector<vertex> verticesLeft{};
	//std::vector<vertex> verticesRight{};
	///* x from -1 to 1*/
	//float increment = ((float)2 / (float)leftData.size());

	//for (int i = 0; i < leftData.size(); ++i)
	//{
	//	float x = -1.0f + (increment * (float)i);
	//	verticesLeft.push_back(vertex{ x,leftData[i] + 0.2f,0.0f });
	//}
	//for (int i = 0; i < rightData.size(); ++i)
	//{
	//	float x = -1.0f + (increment * (float)i) + increment;
	//	verticesRight.push_back(vertex{ x,rightData[i] - 0.2f,0.0f });
	//}
	//float max = buf.amplitude_peak();
	//float average = buf.amplitude_average();
	//float min = buf.amplitude_minimum();
	//std::vector<vertex> maxVerts = std::vector<vertex>{ vertex{ -1.0f,max,0.0f },vertex{ 1.0f,max,0.0f } };
	//std::vector<vertex> averageVerts = std::vector<vertex>{ vertex{ -1.0f,average,0.0f },vertex{ 1.0f,average,0.0f } };
	//std::vector<vertex> minVerts = std::vector<vertex>{ vertex{ -1.0f,min,0.0f },vertex{ 1.0f,min,0.0f } };


	//m_leftScopeElemCount = verticesLeft.size();
	//m_rightScopeElemCount = verticesRight.size();
	//m_maxLineElemCount = maxVerts.size();
	//m_averageLineElemCount = averageVerts.size();
	//m_minLineElemCount = minVerts.size();


	m_timeElemCounts.resize(m_audioChannelCount);
	m_frequencyElemCounts.resize(m_audioChannelCount);
	for (size_t c = 0; c < m_audioChannelCount; ++c)
	{
		m_timeElemCounts[c] = verticesTimeDomain[c].size();
		m_frequencyElemCounts[c] = verticesFrequencyDomain[c].size();
	}

	for (size_t c = 0; c < m_audioChannelCount; ++c)
	{
		/* TIME DOMAIN: Send to GFX */
		glBindVertexArray(m_timeVAO[c]);
		glBindBuffer(GL_ARRAY_BUFFER, m_timeVBO[c]); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)verticesTimeDomain[c].size(), &verticesTimeDomain[c][0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
		glEnableVertexAttribArray(0);

		/* FREQUENCY DOMAIN: Send to GFX */
		glBindVertexArray(m_frequencyVAO[c]);
		glBindBuffer(GL_ARRAY_BUFFER, m_frequencyVBO[c]); /* bind VBO to as current GL_ARRAY_BUFFER. Any subsequent calls modify the data at VBO. */
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*(__int64)verticesFrequencyDomain[c].size(), &verticesFrequencyDomain[c][0], GL_DYNAMIC_DRAW); /* Copy vertex data into buffer. */
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
	}
}

void Visualisation_Oscilloscope::renderFrame()
{
	for (size_t c = 0; c < m_audioChannelCount; ++c)
	{
		m_timeShader->use();
		glBindBuffer(GL_ARRAY_BUFFER, m_timeVBO[c]);
		glBindVertexArray(m_timeVAO[c]);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_POINTS, 0, (GLsizei)m_timeElemCounts[c]);

		m_freqShader->use();
		glBindBuffer(GL_ARRAY_BUFFER, m_frequencyVBO[c]);
		glBindVertexArray(m_frequencyVAO[c]);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_POINTS, 0, (GLsizei)m_frequencyElemCounts[c]);
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Visualisation_Oscilloscope::activate()
{
	m_timeShader = new Shader{ "../Shaders/Oscilloscope_TimeVertex.vs","../Shaders/Oscilloscope_TimeFragment.fs" };
	m_freqShader = new Shader{ "../Shaders/Oscilloscope_FrequencyVertex.vs","../Shaders/Oscilloscope_FrequencyFragment.fs" };

	glGenVertexArrays((GLsizei)m_frequencyVAO.size(), (GLuint*)&m_frequencyVAO[0]);
	glGenBuffers((GLsizei)m_frequencyVBO.size(), (GLuint*)&m_frequencyVBO[0]);

	glGenVertexArrays((GLsizei)m_timeVAO.size(), (GLuint*)&m_timeVAO[0]);
	glGenBuffers((GLsizei)m_timeVBO.size(), (GLuint*)&m_timeVBO[0]);

	m_active = true;
}

void Visualisation_Oscilloscope::deactivate()
{
	glDeleteVertexArrays((GLsizei)m_frequencyVAO.size(), (GLuint*)&m_frequencyVAO[0]);
	glDeleteBuffers((GLsizei)m_frequencyVBO.size(), (GLuint*)&m_frequencyVBO[0]);

	glDeleteVertexArrays((GLsizei)m_timeVAO.size(), (GLuint*)&m_timeVAO[0]);
	glDeleteBuffers((GLsizei)m_timeVBO.size(), (GLuint*)&m_timeVBO[0]);

	delete m_timeShader;
	delete m_freqShader;
	m_timeShader = nullptr;
	m_freqShader = nullptr;

	m_active = false;
}
