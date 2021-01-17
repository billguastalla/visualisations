#pragma once
#include "../maths/Tree.h"
#include "../maths/ODE.h"

#include <vector>
#include <GLM/glm.hpp>
#include <glm/gtc/quaternion.hpp>
/*
	Design limitations currently intended:
		-> Responsibility of trajectory is blurred with mesh generator.
		-> Switch pattern on functions where inheritance could be placed.
		-> TrajectorySettings holds redundant state
		-> Tree is actually persistent, whereas other trajectories vary.
			-> Could be changed with oscillation of tree branches!?
		-> When getting vertices from tree: issue of disjoint vertices from one branch to another.
				-> How do we prevent incorrect velocities being calculated from start of new branch?


	Issues with ODEs:
		-> ODEs need to be 'set off' after which they will evolve.
			-> To continuously produce new trajectories, they need to be fed with their final values.
		-> With the SHO, the realtime timestep is too small!
			-> Sol: timestep rescale?
*/

namespace Trajectory
{

	struct Settings_Attractor
	{
		Settings_Attractor()
			:
			x_0{ 0.5 }, y_0{ 0.5 }, z_0{ 0.5 },
			t_0{ 0.0 },
			t_f{ 10.0 },
			dt{ 0.005 },
			timescale{ 1.f },
			attractorType{AttractorType::Lorenz}
		{}

		enum class AttractorType
		{
			Lorenz,
			RabinovichFabrikant,
			Rossler,
			Multiscroll
		} attractorType;
		static std::string attractorTypeStr(const AttractorType & t);

		double x_0, y_0, z_0;		// tracked position (not translated or rotated by m_pos/m_orient)

		double t_0;		// initial time
		double t_f;		// final time
		double dt;		// timestep size

		// debugging
		double timescale; // time rescaling!

		AttractorODE_Lorenz m_lorenz;
		AttractorODE_RabinovichFabrikant m_rabinovichFabrikant;
		AttractorODE_Rossler m_rossler;
		AttractorODE_Multiscroll m_multiscroll;

		void drawUI();
	};
	std::vector<glm::vec3> generateAttractor(Settings_Attractor& s);

	struct Settings_SHO
	{
		Settings_SHO()
			:
			gamma{ 0.15 },
			x_0{ 1.0 },
			p_0{ 0.0 },
			t_0{ 0.0 },
			t_f{ 10.0 },
			dt{ 0.005 },
			timescale{ 1.f }
		{}
		double gamma;	// linear constant of SHO
		double x_0;		// starting position
		double p_0;		// starting momentum
		double t_0;		// initial time
		double t_f;		// final time
		double dt;		// timestep size

		// debugging
		double timescale; // time rescaling!

		void drawUI();
	};
	std::vector<glm::vec3> generateSHO(Settings_SHO& s);
	struct Settings_Helix
	{
		Settings_Helix()
			:
			t_0{ 0.0 },
			t_f{ 10.0 },
			intervals{ 3u },
			componentAmplitudes{ 1.f },
			componentFrequencies{ 1.f }
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


	constexpr char trajectoryTypeOptions[] = "SHO\0Helix\0Mesh\0Tree\0LorenzAttractor\0HarmonicOscillator\0PlanetarySystem\0SphericalHarmonics";
	struct Settings // temporary design, will switch to simpler one as common data/methods are found.
	{
		Settings() : sho{}, helix{}, tree{}, type{ Type::Helix }, m_position{ 0.f },
			m_orientation{ 0.,0.,0.,1. }
		{}
		enum class Type
		{
			SHO,
			Helix,
			Mesh, // tbc
			Tree,
			LorenzAttractor,
			HarmonicOscillator, // tbc
			PlanetarySystem, // tbc
			SphericalHarmonics // tbc
		} type;
		void setTime(double t_0, double t_f, double dt);
		void drawUI();
		Settings_SHO sho;
		Settings_Attractor lorenz;
		Settings_Helix helix;
		Settings_Tree tree;

		glm::vec3 m_position;		// starting position
		glm::quat m_orientation;	// starting orientation
	};
	std::vector<glm::vec3> generate(Settings& s);

}
