#pragma once
#include "Visualisation.h"

class Shader;
struct GLFWwindow;

class Visualisation_Oscilloscope : Visualisation
{
public:
	Visualisation_Oscilloscope(GLFWwindow * win);
	~Visualisation_Oscilloscope();

	void activate() override;
	void deactivate() override;

	/* Should have two functions:
			-> Process samples (Buffer buf, unsigned samples), generating appropriate gl objects to be drawn later.
					: buf is the audio buffer to use, samples is the number of samples we have moved through (equivalent to time passing).
					: what this means is that we can interpolate, so 512 samples can be passed in a buffer, every 20 samples for instance.
					: as a result, we can tune the offset & sample size from outside of the visualisation object:
						: i.e. : If we want to move the sample window ahead / behind the video we can.
							-> Where this would be controlled is probably in an audio settings class.
			-> Render frame : draw the gl objects to the framebuffer
	*/
	void processSamples(const Buffer & buf, unsigned samples) override;
	void renderFrame() override;

private:
	Shader * m_shader;
	GLFWwindow * m_window;

	unsigned int m_leftScopeVAO, m_leftScopeVBO, m_leftScopeElemCount;
	unsigned int m_rightScopeVAO, m_rightScopeVBO, m_rightScopeElemCount;
	unsigned int m_maxLineVAO, m_maxLineVBO, m_maxLineElemCount;
	unsigned int m_averageLineVAO, m_averageLineVBO, m_averageLineElemCount;
	unsigned int m_minLineVAO, m_minLineVBO, m_minLineElemCount;
};