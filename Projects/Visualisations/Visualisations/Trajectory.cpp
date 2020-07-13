#pragma warning(disable:4996) // ignore sprintf deprecation warnings
#include "Trajectory.h"
#include <boost/numeric/odeint.hpp>
#include <imgui/imgui.h>
using namespace Trajectory;
using namespace boost::numeric;

std::vector<glm::vec3> Trajectory::generateLorenz(Settings_Lorenz& s)
{
	double tempDT{ (s.t_f - s.t_0) * s.dt };
	std::vector<glm::vec3> result{};
	ODE_Lorenz lorenz{};
	lorenz.sigma = s.sigma;
	lorenz.R = s.R;
	lorenz.b = s.b;
	std::vector<state_type> states;
	std::vector<double> times;
	ODE_State obs{ states,times };
	state_type x{};
	x.resize(3);
	x[0] = s.x_0;
	x[1] = s.y_0;
	x[2] = s.z_0;
	size_t steps = odeint::integrate(lorenz, x, s.t_0 * s.timescale, s.t_f * s.timescale, tempDT, obs);
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
		return generateLorenz(s.lorenz);
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

void Trajectory::Settings_Lorenz::drawUI()
{
	glm::vec3 fsigRb{ (float)sigma,(float)R,(float)b };
	glm::vec3 fPos{ (float)x_0,(float)y_0,(float)z_0 };
	float fDt{ (float)dt };
	float tscale{ (float)timescale };
	ImGui::SliderFloat3("sig/r/b", &fsigRb[0], 0.0f, 30.f);
	ImGui::SliderFloat3("Pos", &fPos[0], -10.f, 10.f);
	ImGui::SliderFloat("timestep", &fDt, 0.0001f, 0.1f);
	ImGui::SliderFloat("timescale", &tscale, 1.f, 100.f);
	if (
		fsigRb[0] != sigma ||
		fsigRb[1] != R ||
		fsigRb[2] != b ||
		fPos[0] != x_0 ||
		fPos[1] != y_0 ||
		fPos[2] != z_0
		|| fDt != dt
		|| timescale != tscale)
	{
		sigma = fsigRb[0];
		R = fsigRb[1];
		b = fsigRb[2];
		x_0 = fPos[0];
		y_0 = fPos[1];
		z_0 = fPos[2];
		dt = fDt;
		timescale = tscale;
	}
}
