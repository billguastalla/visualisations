#include "FramebufferSystem.h"
#include "PostProcessing.h"
#include "Shader.h"
FrameBufferSystem::FrameBufferSystem()
	:
	m_frameBufferHandle{ 0u },
	m_textureHandle{0u},
	m_frameBufferResolution{ 1920,1080 },
	m_frameBufferShader{nullptr}
{
	activate();
}

void FrameBufferSystem::activate()
{
	m_frameBufferShader = new Shader("../Shaders/Rescale/rescale.vs", "../Shaders/Rescale/rescale.fs");

	glGenFramebuffers(1, &m_frameBufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferHandle);
	glGenTextures(1, &m_textureHandle);
	glBindTexture(GL_TEXTURE_2D, m_textureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_frameBufferResolution[0], m_frameBufferResolution[1], 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureHandle, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}

void FrameBufferSystem::startFrame()
{
	// first pass
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferHandle);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferHandle);
	//glRenderbufferStorage(GL_FRAMEBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
	//glViewport(0, 0, 1920, 1080); // fb size
}

void FrameBufferSystem::endFrame()
{
	// second pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_frameBufferShader->use();
	//glBindVertexArray(quadVAO);
	//glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, m_textureHandle);
	PostProcessing::renderQuad();
	//glDrawArrays(GL_TRIANGLES, 0, 6);
}
