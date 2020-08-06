#pragma once
#include "Shader.h"

/* Class to handle HDR & Bloom effects */
class PostProcessing
{
public:
	PostProcessing();

	void initialise(int width, int height);
	void deinitialise();

	void frameRenderBegin();
	void frameRenderEnd();

	void setWireframe(bool wEnabled);
	bool wireframe() const { return m_wireframeEnabled; }

	void setHDREnabled(bool hdrEnabled) { m_hdrEnabled = hdrEnabled; }
	bool HDREnabled() const { return m_hdrEnabled; }

	void setExposure(float exposure) { m_exposure = exposure; }
	const float & exposure() const { return m_exposure; }

	/* Strange effects: */
	const glm::vec2& colBufPerc() { return m_colBufPerc; }
	const glm::vec2& rbPerc() { return m_rbPerc; }
	const glm::vec2& pingPongPerc() { return m_pingPongPerc; }
	void setColBufPerc(glm::vec2 colBuf)
	{
		m_colBufPerc = colBuf;
		deinitBuffers();
		initBuffers();
	}
	void setrbPerc(glm::vec2 rb)
	{
		m_rbPerc = rb;
		deinitBuffers();
		initBuffers();
	}
	void setpingpongPerc(glm::vec2 pingpong)
	{
		m_pingPongPerc = pingpong;
		deinitBuffers();
		initBuffers();
	}
	/* Drawing the buffer over the viewport */
	static void renderQuad();
private:
	void initShaders();
	void deinitShaders();
	void initBuffers();
	void deinitBuffers();


	int m_lastFrameWidth, m_lastFrameHeight;


	// Wireframe needs to be here because post-processing needs it always disabled.
	bool m_wireframeEnabled;

	// HDR & Bloom
	bool m_hdrEnabled;
	bool m_bloomEnabled;
	float m_exposure;
	//Shader* m_hdrShader;
	Shader* m_bloomShader;
	Shader * m_blurShader;

	// HDR Framebuffer Object, Colour Buffer, Render buffer
	unsigned int m_hdrFBO, m_colourBuffers[2], m_rboDepth;

	// Bloom: Blurring
	unsigned int m_pingpongFBO[2];
	unsigned int m_pingpongColorbuffers[2];


	/* Strange effects */
	glm::vec2 m_colBufPerc, m_rbPerc, m_pingPongPerc;
};