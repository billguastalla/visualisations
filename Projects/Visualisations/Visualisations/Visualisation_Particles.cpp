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
	ui_particleLifetime{2.f}
	//ui_hSamplesPerFrame{3}
{
}

void Visualisation_Particles::activate()
{
	delete m_particleSet.release();
	Texture particleTexture{ TextureGenerator::loadTexture("particle1024.png") };
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

void Visualisation_Particles::renderFrame(const Camera& camera, const Timestep& ts)
{
	for (auto & ps : m_particleSystems)
		ps.generate(*m_particleSet,ts);
	m_particleSet->clearParticles(ui_particleLifetime);
	m_particleSet->moveParticles(ts.dt().count() * ui_globalSpeed,ui_particleLifetime);
	m_particleSet->draw(camera);
}

void Visualisation_Particles::drawInterface()
{
	unsigned i{ 1 };
	ImGui::Text("Particle Systems");
	if (ImGui::Button("+"))
		m_particleSystems.push_back(ParticleSystem{});
	std::vector<bool> remBtns{};
	for (auto ps : m_particleSystems)
	{
		std::string name{ "S" + std::to_string(i++) };
		ImGui::Text(name.c_str());
		ImGui::SameLine();
		remBtns.push_back(ImGui::Button(std::string{ "-" + name }.c_str()));
	}
	i = 1;
	for (ParticleSystem & ps : m_particleSystems)
	{
		ps.drawUI("Particle System " + std::to_string(i++));
	}
	for (size_t j = 0; j < remBtns.size(); ++j)
		if (remBtns[j] == true)
			m_particleSystems.erase(m_particleSystems.begin() + j); // might not be safe if two buttons pressed at same time

	//m_emissionSettings.drawUI();
	//ImGui::SliderFloat3("Trajectory Amps", &ui_trajectorysinAmp[0], 0.1f, 10.0f);
	//ImGui::SliderFloat3("Trajectory Freqs", &ui_trajectorysinFreq[0], 0.1f, 10.0f);


	ImGui::Text(std::string{ "Particle count: {" + std::to_string(m_particleSet->particleCount()) + "}" }.c_str());
	ImGui::SliderFloat("Global Time Speed", &ui_globalSpeed, 0.1f, 5.f);
	ImGui::SliderFloat("Particle Lifetime", &ui_particleLifetime, 0.5f, 10.f);



	//ImGui::SliderInt("Helix samples per frame", &ui_hSamplesPerFrame, 3, 100);
}
