#pragma warning(disable:4996) // ignore sprintf deprecation warnings
#include "Trajectory.h"
#include <boost/numeric/odeint.hpp>
#include <imgui/imgui.h>
using namespace Trajectory;
using namespace boost::numeric;

constexpr char attractorOptsStr[] = "Lorenz\0RabinovichFabrikant\0Rossler\0Multiscroll\0";

std::vector<glm::vec3> Trajectory::generateAttractor(Settings_Attractor& s)
{
	double tempDT{ (s.t_f - s.t_0) * s.dt };
	std::vector<glm::vec3> result{};
	std::vector<state_type> states{};
	std::vector<double> times{};
	ODE_State obs{ states,times };
	state_type x{};
	x.resize(3);
	x[0] = s.x_0;
	x[1] = s.y_0;
	x[2] = s.z_0;

	size_t steps{ 0u };
	switch (s.attractorType)
	{
	case Settings_Attractor::AttractorType::Lorenz:
		steps = odeint::integrate(s.m_lorenz, x, s.t_0 * s.timescale, s.t_f * s.timescale, tempDT, obs);
		break;
	case Settings_Attractor::AttractorType::RabinovichFabrikant:
		steps = odeint::integrate(s.m_rabinovichFabrikant, x, s.t_0 * s.timescale, s.t_f * s.timescale, tempDT, obs);
		break;
	case Settings_Attractor::AttractorType::Rossler:
		steps = odeint::integrate(s.m_rossler, x, s.t_0 * s.timescale, s.t_f * s.timescale, tempDT, obs);
		break;
	case Settings_Attractor::AttractorType::Multiscroll:
		steps = odeint::integrate(s.m_multiscroll, x, s.t_0 * s.timescale, s.t_f * s.timescale, tempDT, obs);
		break;
	default:
		break;
	}
	for (state_type t : states)
		result.push_back(glm::vec3{ t[0], t[1], t[2] });
	if (!states.empty())
	{
		s.x_0 = states.back()[0];
		s.y_0 = states.back()[1];
		s.z_0 = states.back()[2];
	}
	return result;
}

std::vector<glm::vec3> Trajectory::generateSHO(Settings_SHO& s)
{
	double tempDT{ (s.t_f - s.t_0) / 100. };

	std::vector<glm::vec3> result{};
	ODE_SHO sho{ s.gamma };
	std::vector<state_type> states;
	std::vector<double> times;
	ODE_State obs{ states,times };
	state_type x{};
	x.resize(2);
	x[0] = s.x_0;
	x[1] = s.p_0;
	size_t steps = odeint::integrate(sho, x, s.t_0 * s.timescale, s.t_f * s.timescale, tempDT, obs);
	for (state_type t : states)
		result.push_back(glm::vec3{ t[0], t[1], 0.f });

	if (!states.empty())
	{
		s.x_0 = states.back()[0];
		s.p_0 = states.back()[1];
	}

	return result; // issue: if dt is too long compared to time difference, not enough vertices generated for valid trajectory.
}

std::vector<glm::vec3> Trajectory::generateHelix(const Settings_Helix& s)
{
	std::vector<glm::vec3> result{};
	float increment{ (s.t_f - s.t_0) / (float)s.intervals };
	for (size_t i{ 0 }; i < s.intervals; ++i)
	{
		float t{ increment * (float)i };
		result.push_back(
			glm::vec3{
			(s.componentAmplitudes.x * std::sin(s.componentFrequencies.x * t)),
			(s.componentAmplitudes.y * std::sin(s.componentFrequencies.y * t)),
			(s.componentAmplitudes.z * std::cos(s.componentFrequencies.z * t))
			});
	}
	return result;
}

std::vector<glm::vec3> Trajectory::generate(Settings& s)
{
	switch (s.type)
	{
	case Settings::Type::SHO:
		return generateSHO(s.sho);
	case Settings::Type::Helix:
		return generateHelix(s.helix);
	case Settings::Type::LorenzAttractor:
		return generateAttractor(s.lorenz);
	case Settings::Type::Tree:
		return s.tree.generateVertices();
	default:
		return std::vector<glm::vec3>{};
	}
}

void Trajectory::Settings::setTime(double t_0, double t_f, double dt)
{
	switch (type)
	{
	case Trajectory::Settings::Type::SHO:
		sho.t_0 = t_0;
		sho.t_f = t_f;
		sho.dt = dt;
		break;
	case Trajectory::Settings::Type::Helix:
		helix.t_0 = t_0;
		helix.t_f = t_f;
		break;
	case Trajectory::Settings::Type::Mesh:
		break;
	case Trajectory::Settings::Type::Tree:
		break;
	case Trajectory::Settings::Type::LorenzAttractor:
		lorenz.t_0 = t_0;
		lorenz.t_f = t_f;
		break;
	case Trajectory::Settings::Type::HarmonicOscillator:
		break;
	case Trajectory::Settings::Type::PlanetarySystem:
		break;
	case Trajectory::Settings::Type::SphericalHarmonics:
		break;
	default:
		break;
	}
}

void Trajectory::Settings::drawUI()
{
	switch (type)
	{
	case Trajectory::Settings::Type::SHO:
		sho.drawUI();
		break;
	case Trajectory::Settings::Type::Helix:
		helix.drawUI();
		break;
	case Trajectory::Settings::Type::Mesh:
		break;
	case Trajectory::Settings::Type::Tree:
		tree.drawUI();
		break;
	case Trajectory::Settings::Type::LorenzAttractor:
		lorenz.drawUI();
		break;
	case Trajectory::Settings::Type::HarmonicOscillator:
		break;
	case Trajectory::Settings::Type::PlanetarySystem:
		break;
	case Trajectory::Settings::Type::SphericalHarmonics:
		break;
	default:
		break;
	}
}

void Trajectory::Settings_SHO::drawUI()
{
	float fGamma{ (float)gamma };
	float fX0{ (float)x_0 };
	float fP0{ (float)p_0 };
	float fDt{ (float)dt };
	float tscale{ (float)timescale };
	ImGui::SliderFloat("Gamma", &fGamma, 0.01f, 1.f);
	ImGui::SliderFloat("X0", &fX0, 0.01f, 1.f);
	ImGui::SliderFloat("P0", &fP0, 0.01f, 1.f);
	ImGui::SliderFloat("timestep", &fDt, 0.0001f, 0.002f);
	//ImGui::SliderFloat("integration scale", &iScale, 1.f, 1000.f);
	ImGui::SliderFloat("timescale", &tscale, 1.f, 100.f);
	if (fGamma != gamma || fX0 != x_0 || fP0 != p_0 || fDt != dt
		|| timescale != tscale)
	{
		gamma = fGamma;
		x_0 = fX0;
		p_0 = fP0;
		dt = fDt;
		timescale = tscale;
	}
}

void Trajectory::Settings_Helix::drawUI()
{
	ImGui::SliderFloat3("Amplitudes", &componentAmplitudes[0], 0.1f, 10.0f);
	ImGui::SliderFloat3("Frequencies", &componentFrequencies[0], 0.1f, 10.0f);
}

std::vector<glm::vec3> Trajectory::Settings_Tree::generateVertices() const
{
	return tree.vertices();
}

void Trajectory::Settings_Tree::drawUI()
{
	int temp_nodesPerLayer{ nodesPerLayer };
	int temp_depth{ depth };
	ImGui::SliderInt("Nodes per branching", &nodesPerLayer, 1, 5);
	ImGui::SliderInt("Depth", &depth, 1, 5);
	if (temp_nodesPerLayer != nodesPerLayer || temp_depth != depth)
	{
		tree.clear();
		tree.build(depth, nodesPerLayer);
	}
	//ImGui::SliderInt("Vertices per branch", &depth, 1, 5);
}

std::string Trajectory::Settings_Attractor::attractorTypeStr(const AttractorType& t)
{
	switch (t)
	{
	case Trajectory::Settings_Attractor::AttractorType::Lorenz:
		return std::string{ "Lorenz" };
	case Trajectory::Settings_Attractor::AttractorType::RabinovichFabrikant:
		return std::string{ "RabinovichFabrikant" };
	case Trajectory::Settings_Attractor::AttractorType::Rossler:
		return std::string{ "Rossler" };
	case Trajectory::Settings_Attractor::AttractorType::Multiscroll:
		return std::string{ "Multiscroll" };
	default:
		return std::string{ "Unknown" };
	};
}

void Trajectory::Settings_Attractor::drawUI()
{
	glm::vec3 fPos{ (float)x_0,(float)y_0,(float)z_0 };
	float fDt{ (float)dt };
	float tscale{ (float)timescale };
	ImGui::SliderFloat3("Initial Position", &m_position[0], -20., 20.);
	ImGui::SliderFloat3("Initial Orientation", &m_orientation[0], -1., 1.);
	ImGui::SliderFloat3("tracked Position", &fPos[0], -10.f, 10.f);
	ImGui::SliderFloat("Timestep", &fDt, 0.0001f, 0.1f);
	ImGui::SliderFloat("Timescale", &tscale, 1.f, 100.f);
	ImGui::Combo("Attractor Type", (int*)&attractorType, &attractorOptsStr[0]);
	switch (attractorType)
	{
	case Trajectory::Settings_Attractor::AttractorType::Lorenz:
		m_lorenz.drawUI();
		break;
	case Trajectory::Settings_Attractor::AttractorType::RabinovichFabrikant:
		m_rabinovichFabrikant.drawUI();
		break;
	case Trajectory::Settings_Attractor::AttractorType::Rossler:
		m_rossler.drawUI();
		break;
	case Trajectory::Settings_Attractor::AttractorType::Multiscroll:
		m_multiscroll.drawUI();
		break;
	}

	if (
		fPos[0] != x_0 ||
		fPos[1] != y_0 ||
		fPos[2] != z_0
		|| fDt != dt
		|| timescale != tscale)
	{
		x_0 = fPos[0];
		y_0 = fPos[1];
		z_0 = fPos[2];
		dt = fDt;
		timescale = tscale;
	}
}
