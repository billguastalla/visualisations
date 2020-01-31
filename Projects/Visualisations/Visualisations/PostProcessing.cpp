#include "PostProcessing.h"
#include <glad/glad.h>

PostProcessing::PostProcessing()
	:
	// HDR/Bloom
	m_hdrEnabled{ false },
	m_exposure{ 1.0f },
	m_bloomShader{ nullptr },
	m_blurShader{ nullptr },

	m_bloomEnabled{ false },
	m_pingpongColorbuffers{ 0 },
	m_pingpongFBO{ 0 },

	m_colourBuffers{ 0,0 },
	m_hdrFBO{ 0 },
	m_rboDepth{ 0 },

	m_wireframeEnabled{ false },

	m_lastFrameWidth{ 0 },
	m_lastFrameHeight{ 0 },

	m_colBufPerc{1.0f },
	m_rbPerc{ 1.0f },
	m_pingPongPerc{ 1.0f }
{
}

#include "GLFW/glfw3.h"
void PostProcessing::initialise(int width, int height)
{
	m_lastFrameWidth = width;
	m_lastFrameHeight = height;

	initShaders();
	initBuffers();
}

void PostProcessing::deinitialise()
{
	deinitShaders();
	deinitBuffers();
}

void PostProcessing::frameRenderBegin()
{
	if (m_hdrEnabled)
	{
		/* HDR: BIND FRAMEBUFFER */
		glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	if (m_wireframeEnabled)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


}

void PostProcessing::frameRenderEnd()
{
	if (m_wireframeEnabled)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (m_hdrEnabled)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/* Bloom: Blur image. */
		bool horizontal = true, first_iteration = true;
		unsigned int amount = 20;
		m_blurShader->use();
		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[horizontal]);
			m_blurShader->setInt("horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? m_colourBuffers[1] : m_pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
			renderQuad();
			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_bloomShader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_colourBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_pingpongColorbuffers[!horizontal]);
		m_bloomShader->setInt("bloom", m_hdrEnabled);
		m_bloomShader->setFloat("exposure", m_exposure);
		renderQuad();
	}

}

void PostProcessing::setWireframe(bool wEnabled)
{
	m_wireframeEnabled = wEnabled;
	if (!m_wireframeEnabled) // Unset fill in case, in the future, we have for some reason caught this function inside the render loop
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

void PostProcessing::initShaders()
{
	/* NB: This was taken from: https://learnopengl.com/Advanced-Lighting/Bloom, and refactored */
	/* HDR: Set up shader */
	//m_hdrShader = new Shader{ "../Shaders/Bloom/hdr.vs","../Shaders/Bloom/hdr.fs" };
	if (m_bloomShader == nullptr)
		m_bloomShader = new Shader{ "../Shaders/Bloom/bloom.vs","../Shaders/Bloom/bloom.fs" };
	if (m_blurShader == nullptr)
		m_blurShader = new Shader{ "../Shaders/Bloom/blur.vs", "../Shaders/Bloom/blur.fs" };
	//shader.use();
	//shader.setInt("diffuseTexture", 0);
	m_blurShader->use();
	m_blurShader->setInt("image", 0);
	m_bloomShader->use();
	m_bloomShader->setInt("scene", 0);
	m_bloomShader->setInt("bloomBlur", 1);
}

void PostProcessing::deinitShaders()
{
	if (m_bloomShader != nullptr)
	{
		delete m_bloomShader;
		m_bloomShader = nullptr;
	}
	if (m_blurShader != nullptr)
	{
		delete m_blurShader;
		m_blurShader = nullptr;
	}
}

void PostProcessing::initBuffers()
{
	/* For strange post-proc effects */
	int colBufW = (int)(m_colBufPerc.x * (float)m_lastFrameWidth);
	int colBufH = (int)(m_colBufPerc.y * (float)m_lastFrameHeight);
	int rbW = (int)(m_rbPerc.x * (float)m_lastFrameWidth);
	int rbH = (int)(m_rbPerc.y * (float)m_lastFrameHeight);
	int pingPongW = (int)(m_pingPongPerc.x * (float)m_lastFrameWidth);
	int pingPongH = (int)(m_pingPongPerc.y * (float)m_lastFrameHeight);

	/* HDR: Generate Frame Buffer */
	glGenFramebuffers(1, &m_hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);
	/* HDR+Bloom: Generate Colour Buffers:
		-> These two colour buffers will represend the unmodified render and the blurred render. */
	glGenTextures(2, &m_colourBuffers[0]);
	for (unsigned int i = 0; i < 2; i++) /* <- go through both colour buffer textures */
	{
		/* Bind the colour buffer texture */
		glBindTexture(GL_TEXTURE_2D, m_colourBuffers[i]);
		/* Initialise an image on it, size of viewport */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F,
			m_lastFrameWidth - (i * (m_lastFrameWidth - colBufW)),
			m_lastFrameHeight - (i * (m_lastFrameHeight - colBufH)),
			0, GL_RGB, GL_FLOAT, NULL);
		/* Usual interpolation parameters */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		/* Joey reminds us that clamp to edge is important here because we will be using blur to
		   sample values at edge and don't want to resample same values. */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		/* Attach the colour buffer to the framebuffer, and tell the framebuffer it's a texture
			to write to. */
		glFramebufferTexture2D(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + i,
			GL_TEXTURE_2D,
			m_colourBuffers[i], 0);
	}
	/* HDR+Bloom: Generate Render Buffer */
	glGenRenderbuffers(1, &m_rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, rbW, rbH);
	/* Attach the renderbuffer m_rboDepth, to the framebuffer m_hdrFBO*/
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);

	/* glDrawBuffers "Specifies a list of color buffers to be drawn into".
		so here we are telling the framebuffer that the fragment shader is going to write to
		two indexes 0, and 1. */
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	/* Warn here if framebuffer has missing attachment etc. */
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "RENDERBUFFER: Framebuffer is incomplete" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/* Bloom: Generate Colour Buffers:
		-> These two colour buffers hold vertically and horizontally blurred images.
		-> We generate both framebuffers and textures. Initially retrieving the colour buffer,
		   We will use the render pass to write from framebuffer to texture, to framebuffer,
			each time alternating in horizontal and vertical blurring.
	*/
	glGenFramebuffers(2, m_pingpongFBO);
	glGenTextures(2, m_pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, m_pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F,
			m_lastFrameWidth - (i * (m_lastFrameWidth - pingPongW)),
			m_lastFrameHeight - (i * (m_lastFrameHeight - pingPongH)),
		0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongColorbuffers[i], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "BLUR: Blur framebuffers not complete." << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessing::deinitBuffers()
{
	// ---
	glDeleteFramebuffers(1, &m_hdrFBO);
	glDeleteTextures(2, &m_colourBuffers[0]);

	glDeleteRenderbuffers(1, &m_rboDepth);
	glDeleteFramebuffers(2, &m_pingpongFBO[0]);
	glDeleteTextures(2, &m_pingpongColorbuffers[0]);
	// ---
	m_hdrFBO = 0;
	m_colourBuffers[0] = 0;
	m_colourBuffers[1] = 0;
	m_rboDepth = 0;
	m_pingpongFBO[0] = 0;
	m_pingpongFBO[1] = 0;
	m_pingpongColorbuffers[0] = 0;
	m_pingpongColorbuffers[1] = 0;
}
