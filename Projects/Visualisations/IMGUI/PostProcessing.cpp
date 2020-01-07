#include "PostProcessing.h"
#include <glad/glad.h>

PostProcessing::PostProcessing()
	:
	// HDR/Bloom
	m_hdrEnabled{ false },
	m_exposure{ 1.0f },
	m_hdrShader{ nullptr },

	m_colourBuffer{ 0 },
	m_hdrFBO{ 0 },
	m_rboDepth{ 0 },

	m_wireframeEnabled{ false }
{
}

void PostProcessing::initialise()
{
	/* HDR: Set up shader */
	m_hdrShader = new Shader{ "../Shaders/Bloom/hdr.vs","../Shaders/Bloom/hdr.fs" };

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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colourBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer incomplete" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_hdrShader->use();
	m_hdrShader->setInt("hdrBuffer", 0);
	m_hdrShader->setInt("hdr", m_hdrEnabled);
	m_hdrShader->setFloat("exposure", m_exposure);
}

void PostProcessing::deinitialise()
{
	if (m_hdrShader != nullptr)
	{
		delete m_hdrShader;
		m_hdrShader = nullptr;
	}
}

void PostProcessing::frameRenderBegin()
{
	if (m_hdrEnabled)
	{
		/* HDR: BIND FRAMEBUFFER */
		glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	if(m_wireframeEnabled)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void PostProcessing::frameRenderEnd()
{
	if (m_wireframeEnabled)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (m_hdrEnabled)
	{
		/* HDR: UNBIND FRAMEBUFFER */
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_hdrShader->use();
		m_hdrShader->setInt("hdr", m_hdrEnabled);
		m_hdrShader->setFloat("exposure", m_exposure);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_colourBuffer);
		renderQuad();
	}

}

void PostProcessing::setWireframe(bool wEnabled)
{
	m_wireframeEnabled = wEnabled;
	if(!m_wireframeEnabled) // Unset fill in case, in the future, we have for some reason caught this function inside the render loop
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void PostProcessing::renderQuad()
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
