#pragma once
#include "Visualisation.h"

#include <memory>
#include <vector>

class Settings_Visualisation;
class LockableBuffer;

class Model_Visualisation
{
public:
	Model_Visualisation(std::shared_ptr<Settings_Visualisation> & settings);
	~Model_Visualisation();

	int visualisationOptionsCount();
	int currentVisualisationIndex() { return m_currentVisualisaton; }
	Visualisation* currentVisualisation() { return (m_currentVisualisaton != -1) ? m_visualisations[m_currentVisualisaton] : nullptr; }
	std::string visualisationOptionsString();
	void setVisualisation(int option);
	void runVisualisation();

	void processAudio(const Buffer & buffer);
private:
	std::shared_ptr<Settings_Visualisation> m_settings;
	std::vector<Visualisation*> m_visualisations;
	int m_currentVisualisaton;
};

