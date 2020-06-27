#include "Visualisation_ODE.h"
#pragma warning(disable:4996)
#include <boost/numeric/odeint.hpp>
#include "Texture.h"
//#include "particle_generator.h"
using namespace boost::numeric;

Visualisation_ODE::Visualisation_ODE()
	:
	m_particleShader{nullptr}
{
	
	

	

}

void Visualisation_ODE::activate()
{
	m_particleShader = new Shader{ "../Shaders/Particles_Vertex.vs", "../Shaders/Particles_ObjectFragment.fs" };
}

void Visualisation_ODE::deactivate()
{
	delete m_particleShader;
	m_particleShader = nullptr;
}

void Visualisation_ODE::processSamples(const Buffer& buf, unsigned samples)
{
}


void Visualisation_ODE::renderFrame()
{




}

void Visualisation_ODE::drawInterface()
{
}
