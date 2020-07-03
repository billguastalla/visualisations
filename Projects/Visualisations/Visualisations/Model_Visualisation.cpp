#include "Model_Visualisation.h"

#include "Visualisation_Cubes.h"
#include "Visualisation_Fractal.h"
#include "Visualisation_GameOfLife.h"
#include "Visualisation_Oscilloscope.h"
#include "Visualisation_PointClouds.h"
#include "Visualisation_Sandbox.h"
#include "Visualisation_TreeField.h"
#include "Visualisation_Splinters.h"
#include "Visualisation_Particles.h"

#include "Buffer.h"

#include <GLFW/glfw3.h>
Model_Visualisation::Model_Visualisation(GLFWwindow* win)
	:
	m_visualisations{},
	m_currentVisualisaton{0},
	m_wireframe{false},
	m_postProcessing{ new PostProcessing{} },
	m_window{win}
{
	/* TODO: Make sure any viewport-based stuff is modifiable when viewport is changed!! */
	int w{1920}, h{ 1080 };
	glfwGetWindowSize(win, &w, &h);
	m_postProcessing->initialise(w,h);

	/* At some point you'll want to activate/deactivate on switching modes.*/
	Visualisation_Cubes * visCubes = new Visualisation_Cubes{};
	Visualisation_Oscilloscope * visOscilloscope = new Visualisation_Oscilloscope{};
	Visualisation_PointClouds * visPointClouds = new Visualisation_PointClouds{};
	Visualisation_Fractal * visFractal = new Visualisation_Fractal{};
	Visualisation_Sandbox * visSandbox = new Visualisation_Sandbox{};
	Visualisation_GameOfLife* visGameOfLife = new Visualisation_GameOfLife{};
	Visualisation_TreeField * visTreeField = new Visualisation_TreeField{};
	Visualisation_Splinters* visSplinters = new Visualisation_Splinters{};
	Visualisation_Particles* visParticles = new Visualisation_Particles{};
	/* This has now been delegated to visualisation switching, 
		on account of the increased total memory demands of visualisations.
		See: void Model_Visualisation::setVisualisation(int option)
	*/
	visCubes->activate();

	m_visualisations.push_back(visCubes);
	m_visualisations.push_back(visFractal);
	m_visualisations.push_back(visGameOfLife);
	m_visualisations.push_back(visOscilloscope);
	m_visualisations.push_back(visPointClouds);
	m_visualisations.push_back(visSandbox);
	m_visualisations.push_back(visTreeField);
	m_visualisations.push_back(visSplinters);
	m_visualisations.push_back(visParticles);
}


Model_Visualisation::~Model_Visualisation()
{
	m_postProcessing->deinitialise();

	/* Unset the current visualisation */
	m_currentVisualisaton = -1;

	/* Deactivate and remove all visualisations. */
	for (std::vector<Visualisation*>::iterator i = m_visualisations.begin(); i != m_visualisations.end(); ++i)
		(*i)->deactivate();
	for (std::vector<Visualisation*>::iterator i = m_visualisations.begin(); i != m_visualisations.end(); ++i)
	{
		delete (*i);
		(*i) = nullptr;
	}
	m_visualisations.clear();
}

int Model_Visualisation::visualisationOptionsCount()
{
	return (int)m_visualisations.size();
}

std::string Model_Visualisation::visualisationOptionsString()
{
	std::string result{};
	for (std::vector<Visualisation*>::iterator i = m_visualisations.begin(); i != m_visualisations.end(); ++i)
	{
		std::string title = (*i)->titleString();
		title += '\0';
		result += title;
	}
	return result;
}

void Model_Visualisation::setVisualisation(int option)
{
	if (option == m_currentVisualisaton)
		return;


	if (option < (int)m_visualisations.size() && option >= 0)
	{
		if(currentVisualisation())
			currentVisualisation()->deactivate();
		m_currentVisualisaton = option;
		if (currentVisualisation())
			currentVisualisation()->activate();
	}
	else
		m_currentVisualisaton = -1;



	/* Can activate/deactivate here if you'd like, but currently it's in the constructor/destructor.
		As vis code gets heavier we can move it if needed. */
}

void Model_Visualisation::runVisualisation(const Camera& camera, Timecode t)
{
	if (m_currentVisualisaton != -1)
	{
		Visualisation * currentVis = m_visualisations[m_currentVisualisaton];
		m_postProcessing->frameRenderBegin();
		currentVis->renderFrame(camera,t);
		m_postProcessing->frameRenderEnd();
	}
}

void Model_Visualisation::processAudio(const Buffer & buffer)
{
	if (m_currentVisualisaton != -1)
	{
		Visualisation * currentVis = m_visualisations[m_currentVisualisaton];
		/* You need:
			(a) A better way of passing the buffer through
			(b) An accurate method for monitoring samples passed. (e.g. maybe use some index in the audio interface..)
		*/
		currentVis->processSamples(buffer, 20);
	}
}
