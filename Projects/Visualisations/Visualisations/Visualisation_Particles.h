#pragma once
#include "Visualisation.h"
#include "Shader.h"
#include "ParticleSet.h"
#include <memory>

class Visualisation_Particles : public Visualisation
{
public:
	Visualisation_Particles();

	void activate() override;
	void deactivate() override;
	void processSamples(const Buffer& buf, unsigned samples) override;
	void renderFrame() override;
	std::string titleString() override { return "Particles"; };
	void drawInterface() override;
private:
	std::unique_ptr<ParticleSet> m_particleSet;
	ParticleEmissionSettings m_emissionSettings;

	glm::vec3 ui_trajectorysinAmp;
	glm::vec3 ui_trajectorysinFreq;
	float  ui_globalSpeed;
	int    ui_hSamplesPerFrame;
	double m_lastTime;
};
