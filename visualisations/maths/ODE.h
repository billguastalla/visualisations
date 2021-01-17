#pragma once
#include <vector>

namespace Trajectory
{
	typedef std::vector<double> state_type; // double precision for calculation, single precision for rendering


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

	class AttractorODE_Lorenz
	{
	public:
		AttractorODE_Lorenz()
			: sigma{ 10. }, R{ 28. }, b{ 8. / 3. }{}
		double sigma;
		double R;
		double b;
		void operator() (const state_type& x, state_type& dxdt, const double t)
		{
			dxdt[0] = sigma * (x[1] - x[0]);
			dxdt[1] = R * x[0] - x[1] - x[0] * x[2];
			dxdt[2] = -b * x[2] + x[0] * x[1];
		}
		void drawUI();
	};
	class AttractorODE_RabinovichFabrikant
	{
	public:
		AttractorODE_RabinovichFabrikant()
			: gamma{ 0.87 }, alpha{ 1.1 }{}
		double gamma;
		double alpha;
		void operator() (const state_type& x, state_type& dxdt, const double t)
		{
			// Rabinovich–Fabrikant equations (gamma = 0.87, alpha = 1.1)
			dxdt[0] = x[1] * (x[2] - 1. + (x[0] * x[0])) + (gamma* x[0]);
			dxdt[1] = x[0] * ((3. * x[2]) + 1. - (x[0] * x[0])) + (gamma * x[1]);
			dxdt[2] = -2. * x[2] * (alpha + (x[0] * x[1]));
		}
		void drawUI();
	};
	class AttractorODE_Rossler
	{
	public:
		AttractorODE_Rossler()
			: a_r{ 0.2 }, b_r{ 0.2 }, c_r{ 5.7 }{}
		double a_r, b_r, c_r;
		void operator() (const state_type& x, state_type& dxdt, const double t)
		{
			// Rössler attractor ( a = 0.2, b = 0.2, c = 5.7 )
			dxdt[0] = - x[1] - x[2];
			dxdt[1] = x[0] + (a_r * x[1]);
			dxdt[2] = b_r + (x[2] * (x[0] - c_r));
		}
		void drawUI();
	};
	class AttractorODE_Multiscroll
	{
	public:
		AttractorODE_Multiscroll()
			: a_r{36.}, b_r{3.}, c_r{20.}, u_r{-15.15} {}
		double a_r, b_r, c_r, u_r;
		void operator() (const state_type& x, state_type& dxdt, const double t)
		{
			//// Multiscroll attractors (see modified chua, might have modes!)
			// Lu Chen (a = 36, c = 20, b = 3, u = -15.15), (x(0) = .1, y(0) = .3, z(0) = -.6)
			dxdt[0] = a_r * (x[1] - x[0]);
			dxdt[1] = x[0] - (x[0] * x[2]) + (c_r * x[1]) + u_r;
			dxdt[2] = (x[0] * x[1]) - (b_r * x[2]);
		}
		void drawUI();
	};


	struct ODE_State // holds intermediate steps in an odeint integral calculation
	{
		ODE_State(std::vector<state_type>& states, std::vector<double>& times) : m_states{ states }, m_times{ times } {}
		std::vector<state_type>& m_states;
		std::vector<double>& m_times;
		void operator() (const state_type& x, double t)
		{
			m_states.push_back(x);
			m_times.push_back(t);
		}
	};

};