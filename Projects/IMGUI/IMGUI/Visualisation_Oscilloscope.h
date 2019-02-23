#pragma once
#include "Visualisation.h"
#include <vector>
class Shader;
struct GLFWwindow;

class Visualisation_Oscilloscope : public Visualisation
{
public:
	Visualisation_Oscilloscope();
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

	std::string titleString() override { return "Oscilloscope"; };
private:
	Shader * m_timeShader, * m_freqShader;

	/* idx is channel*/
	std::vector<unsigned int> m_frequencyVAO;
	std::vector<unsigned int> m_frequencyVBO;
	std::vector<size_t> m_frequencyElemCounts;
	std::vector<unsigned int> m_timeVAO;
	std::vector<unsigned int> m_timeVBO;
	std::vector<size_t> m_timeElemCounts;

	size_t m_audioChannelCount;
};