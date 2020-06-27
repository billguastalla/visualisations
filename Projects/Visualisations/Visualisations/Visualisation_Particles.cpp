#include "Visualisation_Particles.h"
#pragma warning(disable:4996)
#include <boost/numeric/odeint.hpp>
#include "Texture.h"
//#include "particle_generator.h"
using namespace boost::numeric;

Visualisation_Particles::Visualisation_ODE()
	:
	m_particleShader{nullptr}
{
	
	

	

}

void Visualisation_Particles::activate()
{
	m_particleShader = new Shader{ "../Shaders/Particles_Vertex.vs", "../Shaders/Particles_ObjectFragment.fs" };
}

void Visualisation_Particles::deactivate()
{
	delete m_particleShader;
	m_particleShader = nullptr;
}

void Visualisation_Particles::processSamples(const Buffer& buf, unsigned samples)
{
}


void Visualisation_Particles::renderFrame()
{




}

void Visualisation_Particles::drawInterface()
{
}
