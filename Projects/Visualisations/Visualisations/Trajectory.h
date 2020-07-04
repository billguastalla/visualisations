#pragma once
#include "Tree.h"
#include <vector>
#include <GLM/glm.hpp>
/*
	Design limitations currently intended:
		-> Responsibility of trajectory is blurred with mesh generator.
		-> Switch pattern on functions where inheritance could be placed.
		-> TrajectorySettings holds redundant state
		-> Tree is actually persistent, whereas other trajectories vary.
			-> Could be changed with oscillation of tree branches!?
		-> When getting vertices from tree: issue of disjoint vertices from one branch to another.
				-> How do we prevent incorrect velocities being calculated from start of new branch?
*/

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

	struct ODE_State // holds intermediate steps in an odeint integral calculation
	{
		ODE_State() : m_states{}, m_times{} {}
		std::vector<state_type> m_states;
		std::vector<double> m_times;
		void operator() (const state_type& x, double t)
		{
			m_states.push_back(x);
			m_times.push_back(t);
		}
	};

	struct Settings_SHO
	{
		Settings_SHO()
			:
			gamma{ 0.15 },
			x_0{ 1.0 },
			p_0{ 0.0 },
			t_0{ 0.0 },
			t_f{ 10.0 },
			dt{ 0.005 }
		{}
		double gamma;	// linear constant of SHO
		double x_0;		// starting position
		double p_0;		// starting momentum
		double t_0;		// initial time
		double t_f;		// final time
		double dt;		// timestep size
		void drawUI();
	};
	std::vector<glm::vec3> generateSHO(const Settings_SHO& s);
	struct Settings_Helix
	{
		Settings_Helix()
			:
			t_0{ 0.0 },
			t_f{ 10.0 },
			intervals{ 3u },
			componentAmplitudes{1.f},
			componentFrequencies{1.f}
		{}
		float t_0;
		float t_f;
		size_t intervals; // should be turned into dt !
		glm::vec3 componentAmplitudes;		// prefactor for helix components
		glm::vec3 componentFrequencies;		// turning rate for helix components
		void drawUI();
	};
	std::vector<glm::vec3> generateHelix(const Settings_Helix& s);


	struct Settings_Tree
	{
		Settings_Tree() : nodesPerLayer{ 2 }, depth{ 2 }, verticesPerBranch{ 5 }, tree{} {}

		std::vector<glm::vec3> generateVertices() const;
		void drawUI();
		Tree tree;
		int nodesPerLayer; // 1 to N
		int depth; // 1 to N
		int verticesPerBranch; // 1 to N
	};


	constexpr char trajectoryTypeOptions[] = "SHO\0Helix\0Mesh\0Tree\0LorentzAttractor\0HarmonicOscillator\0PlanetarySystem\0SphericalHarmonics";
	struct Settings // temporary design, will switch to simpler one as common data/methods are found.
	{
		Settings() : sho{}, helix{}, tree{}, type{ Type::Helix } {}
		enum class Type
		{
			SHO,
			Helix,
			Mesh, // tbc
			Tree, // tbc
			LorentzAttractor, // tbc
			HarmonicOscillator, // tbc
			PlanetarySystem, // tbc
			SphericalHarmonics // tbc
		} type;
		void setTime(double t_0, double t_f, double dt);
		void drawUI();
		Settings_SHO sho;
		Settings_Helix helix;
		Settings_Tree tree;
	};
	std::vector<glm::vec3> generate(const Settings& s);

}
