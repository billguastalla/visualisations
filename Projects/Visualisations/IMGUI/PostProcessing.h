#pragma once
#include "Shader.h"

/* Class to handle HDR & Bloom effects */
class PostProcessing
{
public:
	PostProcessing();

	void initialise();
	void deinitialise();

	void frameRenderBegin();
	void frameRenderEnd();

	void setWireframe(bool wEnabled);
	bool wireframe() const { return m_wireframeEnabled; }

	void setHDREnabled(bool hdrEnabled) { m_hdrEnabled = hdrEnabled; }
	bool HDREnabled() const { return m_hdrEnabled; }

	void setExposure(float exposure) { m_exposure = exposure; }
	const float & exposure() const { return m_exposure; }
private:
	/* Drawing the buffer over the viewport */
	static void renderQuad();

	// Wireframe needs to be here because post-processing needs it always disabled.
	bool m_wireframeEnabled;

	// HDR & Bloom
	bool m_hdrEnabled;
	float m_exposure;
	Shader * m_hdrShader;

	// HDR Framebuffer Object, Colour Buffer, Render buffer
	unsigned int m_hdrFBO, m_colourBuffer, m_rboDepth;
};