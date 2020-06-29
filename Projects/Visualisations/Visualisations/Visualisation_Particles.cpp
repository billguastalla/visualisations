#include "Visualisation_Particles.h"
#include "Texture.h"
#include <GLFW\glfw3.h>
#include <imgui/imgui.h>

/*
	TODO:
		1. Lorenz attractor
		2. Tree
		3. Spherical Harmonics
		4. Mesh/Plane
*/
Visualisation_Particles::Visualisation_Particles()
	:
	m_particleSet{nullptr},
	m_lastTime{0.},

	ui_globalSpeed{1.0f},
	ui_hSamplesPerFrame{3},
	ui_trajectorysinAmp{1.f},
	ui_trajectorysinFreq{1.f}
{
	m_camera.m_position = glm::vec3{ 0.0,0.0,3.0 };
}

void Visualisation_Particles::activate()
{
	delete m_particleSet.release();
	Texture particleTexture{ TextureGenerator::loadTexture("particle.png") };
	m_particleSet.reset(new ParticleSet{particleTexture});

	m_lastTime = glfwGetTime();
}

void Visualisation_Particles::deactivate()
{
	delete m_particleSet.release();
}

void Visualisation_Particles::processSamples(const Buffer& buf, unsigned samples)
{
}

void Visualisation_Particles::renderFrame()
{
	double currentTime{ glfwGetTime() };

	Trajectory::Settings_Helix s{};
	s.t_0 = m_lastTime;
	s.t_f = currentTime;
	s.intervals = ui_hSamplesPerFrame;
	s.componentAmplitudes = ui_trajectorysinAmp;
	s.componentFrequencies = ui_trajectorysinFreq;
	std::vector<glm::vec3> trajectory{ Trajectory::generateHelix(s) };

	m_particleSet->generateParticles(
		trajectory, m_emissionSettings);
	m_particleSet->clearParticles();
	m_particleSet->moveParticles((currentTime - m_lastTime) * ui_globalSpeed);

	m_particleSet->draw(m_camera);

	m_lastTime = currentTime;
}

void Visualisation_Particles::drawInterface()
{
	ImGui::Text(std::string{ "Camera Pos" + std::to_string(m_camera.m_position.x) + "," +
	std::to_string(m_camera.m_position.y) + "," + std::to_string(m_camera.m_position.z) }.c_str());

	ImGui::Text("Emission Settings:");
	ImGui::SliderFloat("\tTravel Distance Mean", &m_emissionSettings.m_meanTravelDist, 1.f, 10.0f);
	ImGui::SliderFloat("\tTravel Distance Sigma", &m_emissionSettings.m_sigmaTravelDist, 0.03f, 1.0f);
	ImGui::SliderFloat("\tVelocity Decay Rate Base", &m_emissionSettings.m_baseDecayRate, 0.1f, 5.f);
	ImGui::SliderFloat("\tGlobal Particle Scale", &m_emissionSettings.m_globalParticleScale, 0.1f, 1.f);
	ImGui::SliderFloat("\tGlobal Velocity", &m_emissionSettings.m_globalVelocity, 0.1f, 10.f);

	ImGui::Combo("Emission Direction", (int*)&m_emissionSettings.m_emissionDirection, &emissionOptions[0]);
	ImGui::Combo("Trajectory Type", (int*)&m_trajectorySettings.type, &Trajectory::trajectoryTypeOptions[0]);

	ImGui::SliderFloat("Global Time Speed", &ui_globalSpeed, 0.1f, 5.f);
	ImGui::SliderInt("Helix samples per frame", &ui_hSamplesPerFrame, 3, 100);

	ImGui::SliderFloat3("Trajectory Amps", &ui_trajectorysinAmp[0], 0.1f, 10.0f);
	ImGui::SliderFloat3("Trajectory Freqs", &ui_trajectorysinFreq[0], 0.1f, 10.0f);
}
