#include "Visualisation_PointClouds.h"

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

Visualisation_PointClouds::Visualisation_PointClouds()
	:
	m_lightPos{ 0.0f,0.0f,0.0f },

	m_objectShader{ nullptr },
	m_lampShader{ nullptr },

	m_cubeColours{},
	m_cubePositions{}
{
}

void Visualisation_PointClouds::activate()
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

void Visualisation_PointClouds::deactivate()
{
	glDeleteVertexArrays(1, &m_cubeVAO);
	glDeleteVertexArrays(1, &m_lightVAO);
	glDeleteBuffers(1, &m_cubeVBO);

	m_cubePositions.clear();
	m_cubeColours.clear();
	m_vertices.clear();

	delete m_lampShader;
	m_lampShader = nullptr;
	delete m_objectShader;
	m_objectShader = nullptr;

	m_active = false;
}

void Visualisation_PointClouds::processSamples(const Buffer & buf, unsigned samples)
{
	MetaDistribution<double> dist{ 0.0f,0.01f,0.1f,0.01f };

	std::vector<std::vector<kiss_fft_cpx>> res = buf.fft();
	std::vector<std::vector<kiss_fft_cpx>> reduced;
	reduced.resize(res.size());
	for (int i = 0; i < res.size(); ++i)
	{
		// Take Middle
		int size = res[i].size();
		for (int j = (size / 4); j < (size * 3.0 / 4.0); ++j)
			reduced[i].push_back(res[i][j]);
	}
	//Buffer::normaliseFFT(reduced);
	double maxIVal = 0.0f;
	//std::vector<kiss_fft_cpx>::iterator maxI = std::find_if(reduced[0].begin(), reduced[0].end(),
	//	[maxIVal](bool a, bool b) 
	//{ return false; });

	AbstractRandomObject a;

	std::normal_distribution<double> metaDistX{ dist.createDistribution() };
	std::normal_distribution<double> metaDistY{ dist.createDistribution() };
	std::normal_distribution<double> metaDistZ{ dist.createDistribution() };
	double metaOffsetX{ metaDistX(a.mersenneTwister()) }, metaOffsetY{ metaDistY(a.mersenneTwister()) }, metaOffsetZ{ metaDistZ(a.mersenneTwister()) };


	std::map<int, std::deque<float>> & data = buf.data();
	std::deque<float> & ch = data.at(0);

	for (int i = 0; i < 5; ++i)
	{
		int posX = i;
		int posY = (reduced[0].size() / 2) + i;
		int posZ = (reduced[0].size() - 1) - i;
		double meanX = reduced[0][posX].r;
		double meanY = reduced[0][posY].r;
		double meanZ = reduced[0][posZ].r;
		double rotateX = (i % 3 == 0) ? meanX : ((i % 3 == 1) ? meanY : meanZ);
		double rotateY = (i % 3 == 0) ? meanY : ((i % 3 == 1) ? meanZ : meanX);
		double rotateZ = (i % 3 == 0) ? meanZ : ((i % 3 == 1) ? meanX : meanY);

		double normalisation = std::max(std::max(std::abs(meanX), std::abs(meanY)), std::abs(meanZ));

		double colX{ 0.0 }, colY{ 0.0 }, colZ{ 0.0 };
		for (unsigned i = 0u; i < ch.size(); ++i)
		{
			if (i % 3 == 0)
				if (colX < ch[i])
					colX = ch[i];
			if (i % 7 == 0)
				if (std::abs(colY) < std::abs(ch[i]))
					colY = abs(ch[i]);
			if (i % 2 == 0)
				if (colZ < ch[i])
					colZ = ch[i];
		}
		/* Put a sin curve to increase intensity of lower values */
		//colX = sin((3.141 / 2.0) * colX * colY);
		//colY = sin((3.141 / 2.0) * colY * colZ);
		//colZ = sin((3.141 / 2.0) * colZ * colX);
		std::normal_distribution<double> xCol{ colX,0.1 };
		std::normal_distribution<double> yCol{ colY,0.2 };
		std::normal_distribution<double> zCol{ colZ,0.4 };

		glm::vec3 cubeColour{ std::abs(xCol(a.mersenneTwister())), std::abs(yCol(a.mersenneTwister())),  std::abs(zCol(a.mersenneTwister())) };

		std::normal_distribution<double> xPos{ rotateX, 0.02f + std::abs(metaOffsetX) };
		std::normal_distribution<double> yPos{ rotateY, 0.02f + std::abs(metaOffsetY) };
		std::normal_distribution<double> zPos{ rotateZ, 0.02f + std::abs(metaOffsetZ) };
		glm::vec3 cubePos{ xPos(a.mersenneTwister()),yPos(a.mersenneTwister()), zPos(a.mersenneTwister()) };

		m_cubePositions.insert(m_cubePositions.begin(), cubePos);
		m_cubeColours.insert(m_cubeColours.begin(), cubeColour);
	}

	double signalPowerVal{ 0.0 };
	for (int i = 0; i < ch.size(); ++i)
		signalPowerVal += ((double)ch[i] * ch[i]);
	signalPowerVal /= (double)ch.size();

	m_signalPowerHistory.push_back(signalPowerVal);
	if (m_signalPowerHistory.size() > 10)
		m_signalPowerHistory.pop_front();

	double signalPowerAverage{ 0.0 };
	for (size_t i = 0; i < m_signalPowerHistory.size(); ++i)
		signalPowerAverage += m_signalPowerHistory[i];

	signalPowerAverage /= (double)m_signalPowerHistory.size();


	/* TODO: Figure out a good relationship for the decay rate.. */
	while (m_cubePositions.size() > 500)
	{
		m_cubePositions.pop_back();
		m_cubeColours.pop_back();
	}

	if (m_cubeColours.size() != m_cubePositions.size())
		m_cubeColours.resize(m_cubePositions.size(), glm::vec3{ 1.0f,0.0f,0.0f });

	/*

	//double radians = glm::radians(glfwGetTime());
	//double xPos = sin(radians);
	//double zPos = cos(radians);
	*/
	//m_camera.m_position.x = 3 * sin(glfwGetTime());
	//m_camera.m_position.z = 3 * cos(glfwGetTime());
	//double yawRadians = atan2(m_camera.m_position.x, m_camera.m_position.z);
	//double yawDegrees = glm::degrees(yawRadians);

	//glm::vec3 front;
	//front.x = -m_camera.m_position.x;
	//front.y = -m_camera.m_position.y;
	//front.z = -m_camera.m_position.z;
	//m_camera.m_front = glm::normalize(front);
	//double yaw = glfwGetTime();
	//m_camera.Yaw = 180.0 - yawDegrees;

	//m_camera.updateCameraVectors();


	//float bufPeak = buf.amplitude_peak();
	//float bufAverage = buf.amplitude_average();
	//float bufMinimum = buf.amplitude_minimum();
	//
	////m_camera.Yaw = (bufMinimum * 180.0f) - 90.0f;
	//std::vector<std::vector<kiss_fft_cpx>> fftResults{ buf.fft() };

	/* LHS is fractional position in fft, RHS is magnitude*/
	//std::vector<std::pair<float, float>> peaks{ std::make_pair<float,float>(0.0f, 0.0f),std::make_pair<float,float>(0.0f, 0.0f) };
	//for (int c = 0; c < fftResults.size(); ++c)
	//{
	//	for (int i = 0; i < fftResults[c].size(); ++i)
	//		if (fftResults[c][i].r >= peaks[c].second)
	//		{
	//			peaks[c].first = ((float)i / fftResults[c].size());
	//			peaks[c].second = fftResults[c][i].r;
	//		}
	//}
	//m_objectShader->use();
	//m_objectShader->setVec3("objectColour", glm::vec3{ peaks[0].first,peaks[1].first,1.0f });

	//m_camera.Pitch = bufAverage;
	//m_camera.m_zoom = -45.0f * bufMinimum;
	//m_camera.m_zoom = bufMinimum;

	//buf.maxChannelFrameCount();
	//kiss_fft_cfg config;
	//m_camera.updateCameraVectors();
}

void Visualisation_PointClouds::renderFrame()
{
	// per-frame time logic
	// --------------------
	float currentFrame = glfwGetTime();
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;

	//// input
	//// -----
	//processInput(window);

	//// render
	//// ------
	//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	m_lightPos = glm::vec3(10 * sin(glfwGetTime()), 10 * cos(glfwGetTime()), 10 * sin(glfwGetTime()));
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
		model = glm::scale(model, glm::vec3{ 0.05f,0.05f,0.05f });

		//*m_cubeColours[i].r * m_cubeColours[i].g * m_cubeColours[i].g
		float angle = 0.4f * (float)i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
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
