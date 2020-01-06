#include "Visualisation_Cubes.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Buffer.h"

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
	m_cubePositions{},

	// HDR/Bloom
	m_hdrEnabled{true},
	m_exposure{1.0f},
	m_hdrShader{ nullptr }
{
}

void Visualisation_Cubes::activate()
{
	/* HDR: Set up shader */
	m_hdrShader = new Shader{"../Shaders/Bloom/hdr.vs","../Shaders/Bloom/hdr.fs" };

	/* HDR: Generate Frame Buffer */
	glGenFramebuffers(1, &m_hdrFBO);
	glGenTextures(1, &m_colourBuffer);
	/* HDR: Generate Colour Buffer (2D Texture) */
	glBindTexture(GL_TEXTURE_2D, m_colourBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/* HDR: Generate Render Buffer */
	glGenRenderbuffers(1, &m_rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1920, 1080);
	/* HDR: Attach buffer */
	glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colourBuffer,0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer incomplete" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_hdrShader->use();
	m_hdrShader->setInt("hdrBuffer", 0);



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
}

void Visualisation_Cubes::processSamples(const Buffer & buf, unsigned samples)
{
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

void Visualisation_Cubes::renderFrame()
{
	float currentFrame = (float)glfwGetTime();
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;


	/* HDR: BIND FRAMEBUFFER */
	glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	m_lightPos = glm::vec3(5 * sin(0.1 * glfwGetTime()), 10 * cos(glfwGetTime()), 10 * sin(glfwGetTime()));
	lightModel = glm::translate(lightModel, m_lightPos);
	lightModel = glm::scale(lightModel, glm::vec3{ 0.2f });
	m_objectShader->setVec3("lightPos", m_lightPos);
	m_objectShader->setVec3("viewPos", m_camera.m_position);
		   
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

	/* HDR: UNBIND FRAMEBUFFER */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_hdrShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_colourBuffer);
	m_hdrShader->setInt("hdr", m_hdrEnabled);
	m_hdrShader->setFloat("exposure", m_exposure);
	renderQuad();

}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void Visualisation_Cubes::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

////// Bloom, later:

////*
//Here:
//	-> Make a framebuffer,
//	-> Make two textures, one for original render, second for brightness/blur
//	-> Instantiate screen-sized and screen formatted images in textures
//	-> Setup normal rules for textures: linear interpolation and clamp to edges.
//	-> Connect the textures to the framebuffer.
//*/
//unsigned int hdrFBO{ 0 };
//glGenFramebuffers(1, &hdrFBO);
//glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
//unsigned int colourBuffers[2];
//glGenTextures(2, colourBuffers);
//for (unsigned int i = 0; i < 2; ++i)
//{
//	glBindTexture(GL_TEXTURE_2D, colourBuffers[i]);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F,
//		1920, 1080, 0, GL_RGB, GL_FLOAT, nullptr);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
//		GL_TEXTURE_2D, colourBuffers[i],0);
//}
//// Telling OpenGL that we're rendering to multiple colourbuffers.
//unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
//glDrawBuffers(2, attachments);
