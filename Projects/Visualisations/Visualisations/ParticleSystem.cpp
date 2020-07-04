#include "ParticleSystem.h"
#include <imgui/imgui.h>

ParticleSystem::ParticleSystem()
	: m_emissionSettings{}, m_position{0.f}, m_trajectorySettings{}
{
}

void ParticleSystem::generate(ParticleSet& set, double t_last, double t_current)
{
	//Trajectory::Settings_Helix s{};
	//s.t_0 = m_lastTime;
	//s.t_f = currentTime;
	//s.intervals = ui_hSamplesPerFrame;
	//s.componentAmplitudes = ui_trajectorysinAmp;
	//s.componentFrequencies = ui_trajectorysinFreq;
	m_trajectorySettings.setTime(t_last, t_current, 0.1);
	std::vector<glm::vec3> trajectory{ Trajectory::generate(m_trajectorySettings) };
	if(trajectory.size() > 2)
		set.generateParticles(trajectory, m_emissionSettings);
}

void ParticleSystem::drawUI(const std::string& name)
{
	ImGui::Begin(name.c_str());
	m_emissionSettings.drawUI();
	m_trajectorySettings.drawUI();
	ImGui::Combo("Emission Direction", (int*)&m_emissionSettings.m_emissionDirection, &emissionOptions[0]);
	ImGui::Combo("Trajectory Type", (int*)&m_trajectorySettings.type, &Trajectory::trajectoryTypeOptions[0]);

	ImGui::End();
}
