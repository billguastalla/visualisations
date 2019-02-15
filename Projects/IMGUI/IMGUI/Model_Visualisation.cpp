#include "Model_Visualisation.h"

#include "Visualisation_Cubes.h"
#include "Visualisation_Oscilloscope.h"
#include "Buffer.h"

Model_Visualisation::Model_Visualisation(std::shared_ptr<Settings_Visualisation> & settings)
	:
	m_settings{settings},
	m_visualisations{},
	m_currentVisualisaton{0}
	//m_vis_oscilloscope{},
	//m_vis_cubes{}
{
	//m_vis_cubes.activate(); /* At some point you'll want to activate/deactivate on switching modes.*/
	//m_vis_oscilloscope.activate();
	Visualisation_Cubes * visCubes = new Visualisation_Cubes{};
	Visualisation_Oscilloscope * visOscilloscope = new Visualisation_Oscilloscope{};

	visCubes->activate();
	visOscilloscope->activate();

	m_visualisations.push_back(visCubes);
	m_visualisations.push_back(visOscilloscope);
}

Model_Visualisation::~Model_Visualisation()
{
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
		m_currentVisualisaton = option;
	else
		m_currentVisualisaton = -1;

	/* Can activate/deactivate here if you'd like, but currently it's in the constructor/destructor.
		As vis code gets heavier we can move it if needed. */
}

void Model_Visualisation::runVisualisation()
{
	if (m_currentVisualisaton != -1)
	{
		Visualisation * currentVis = m_visualisations[m_currentVisualisaton];
		currentVis->renderFrame();
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
