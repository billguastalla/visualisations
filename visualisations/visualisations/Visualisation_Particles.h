#pragma once
#include "Visualisation.h"

#include "../particles/ParticleSystem.h"

#include <memory>

class Visualisation_Particles : public Visualisation
{
public:
	Visualisation_Particles();

	void activate() override;
	void deactivate() override;
	void processSamples(const Buffer& buf, unsigned samples) override;
	void renderFrame(const Camera& camera, const Timestep& t) override;
	std::string titleString() override { return "Particles"; };
	void drawInterface() override;
private:

	std::vector<ParticleSystem> m_particleSystems;
	std::unique_ptr<ParticleSet> m_particleSet;

	//ParticleEmissionSettings m_emissionSettings;
	//Trajectory::Settings m_trajectorySettings;
	//glm::vec3 ui_trajectorysinAmp;
	//glm::vec3 ui_trajectorysinFreq;

	float  ui_globalSpeed;
	float ui_particleLifetime;
	//int    ui_hSamplesPerFrame;
	double m_lastTime;

};
