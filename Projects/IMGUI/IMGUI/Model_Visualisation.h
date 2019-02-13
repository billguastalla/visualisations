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
	int currentVisualisation() { return m_currentVisualisaton; }
	std::string visualisationOptionsString();
	void setVisualisation(int option);
	void runVisualisation();

	void setBuffer(std::shared_ptr<LockableBuffer> & buffer);
private:
	std::shared_ptr<Settings_Visualisation> m_settings;
	std::vector<Visualisation*> m_visualisations;
	int m_currentVisualisaton;

	std::shared_ptr<LockableBuffer> p_buffer;
};

