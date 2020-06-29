#pragma once
#include <vector>

struct TrajectorySettings
{
	enum class TrajectoryType
	{
		Helix,
		Tree,
		LorentzAttractor,
		HarmonicOscillator,
		PlanetarySystem,
		SphericalHarmonics
	};
};


typedef std::vector<double> state_type;

class ODE_SHO
{
	double m_gam;

public:
	ODE_SHO(double gam) : m_gam{ gam } {}

	void operator() (const state_type& x, state_type& dxdt, const double t)
	{
		dxdt[0] = x[1];
		dxdt[1] = -x[0] - (m_gam * x[1]);
	}
};

struct ODE_State
{
	std::vector<state_type>& m_states;
	std::vector<double>& m_times;

	ODE_State(std::vector<state_type>& states, std::vector<double>& times)
		:
		m_states{ states },
		m_times{ times }
	{}
	void operator() (const state_type& x, double t)
	{
		m_states.push_back(x);
		m_times.push_back(t);
	}
};