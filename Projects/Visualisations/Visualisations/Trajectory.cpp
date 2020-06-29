#pragma warning(disable:4996) // ignore sprintf deprecation warnings
#include "Trajectory.h"
#include <boost/numeric/odeint.hpp>
using namespace Trajectory;
using namespace boost::numeric;

std::vector<glm::vec3> Trajectory::generateSHO(const Settings_SHO& s)
{
	std::vector<glm::vec3> result{};
	ODE_SHO sho{ s.gamma };
	ODE_State obs{};
	state_type x{ 2 };
	x[0] = s.x_0;
	x[1] = s.p_0;
	size_t steps = odeint::integrate(sho, x, s.t_0, s.t_f, s.dt, obs);
	return result;

	for (state_type t : obs.m_states)
		result.push_back(glm::vec3{ t[0], t[1], 0.f });
	return result;
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

std::vector<glm::vec3> Trajectory::generate(const Settings& s)
{
	switch (s.type)
	{
	case Settings::Type::SHO:
		return generateSHO(s.sho);
	case Settings::Type::Helix:
		return generateHelix(s.helix);
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

		break;
	case Trajectory::Settings::Type::Mesh:
		break;
	case Trajectory::Settings::Type::Tree:
		break;
	case Trajectory::Settings::Type::LorentzAttractor:
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
