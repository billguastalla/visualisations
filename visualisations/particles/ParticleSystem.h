#pragma once
#include "../time/Timekeeping.h"
#include "ParticleSet.h"
#include "Trajectory.h"

class ParticleSystem
{
public:
	ParticleSystem();

	void generate(ParticleSet & set, const Timestep& ts);
	void drawUI(const std::string & name);
private:
	glm::vec3 m_position;
	ParticleEmissionSettings m_emissionSettings;
	Trajectory::Settings m_trajectorySettings;
};