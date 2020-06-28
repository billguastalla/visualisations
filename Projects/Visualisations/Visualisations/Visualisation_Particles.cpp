#include "Visualisation_Particles.h"
#pragma warning(disable:4996)
#include <boost/numeric/odeint.hpp>
#include "Texture.h"
#include <GLFW\glfw3.h>
#include <imgui/imgui.h>

//#include "particle_generator.h"
using namespace boost::numeric;

Visualisation_Particles::Visualisation_Particles()
	:
	m_particleSet{nullptr},
	m_lastTime{0.},


	ui_globalSpeed{1.0f},
	ui_hSamplesPerFrame{3},
	ui_velBaseDecayRate{1.f},
	ui_travelDistanceMean{1.f},
	ui_travelDistanceSigma{1.f},
	ui_globalParticleScale{1.f},

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

	std::vector<glm::vec3> trajectory{ ParticleSet::sampleHelicalTrajectory(m_lastTime, currentTime, ui_hSamplesPerFrame,
		ui_trajectorysinAmp,ui_trajectorysinFreq) };
	m_particleSet->generateParticles(trajectory, ui_travelDistanceMean, ui_travelDistanceSigma, ui_velBaseDecayRate, ui_globalParticleScale);
	m_particleSet->clearParticles();
	m_particleSet->moveParticles((currentTime - m_lastTime) * ui_globalSpeed);

	m_particleSet->draw(m_camera);

	m_lastTime = currentTime;
}

void Visualisation_Particles::drawInterface()
{
	ImGui::Text(std::string{ "Camera Pos" + std::to_string(m_camera.m_position.x) + "," +
	std::to_string(m_camera.m_position.y) + "," + std::to_string(m_camera.m_position.z) }.c_str());

	ImGui::SliderFloat("Travel Distance Mean", &ui_travelDistanceMean, 1.f, 10.0f);
	ImGui::SliderFloat("Travel Distance Sigma", &ui_travelDistanceSigma, 0.03f, 1.0f);


	ImGui::SliderFloat("Global Time Speed", &ui_globalSpeed, 0.1f, 5.f);
	ImGui::SliderInt("Helix samples per frame", &ui_hSamplesPerFrame, 3, 100);
	ImGui::SliderFloat("Velocity Decay Rate Base", &ui_velBaseDecayRate, 0.1f, 5.f);
	ImGui::SliderFloat("Global Particle Scale", &ui_globalParticleScale, 0.1f, 1.f);

	ImGui::SliderFloat3("Trajectory Amps", &ui_trajectorysinAmp[0], 0.1f, 10.0f);
	ImGui::SliderFloat3("Trajectory Freqs", &ui_trajectorysinFreq[0], 0.1f, 10.0f);
}
