#pragma once
#include "Visualisation.h"
#include "PostProcessing.h"

#include <memory>
#include <vector>

class Settings_Visualisation;
class LockableBuffer;
struct GLFWwindow;

class Model_Visualisation
{
public:
	Model_Visualisation(GLFWwindow * win);
	~Model_Visualisation();

	int visualisationOptionsCount();
	int currentVisualisationIndex() { return m_currentVisualisaton; }
	Visualisation* currentVisualisation() { return (m_currentVisualisaton != -1) ? m_visualisations[m_currentVisualisaton] : nullptr; }
	std::string visualisationOptionsString();
	void setVisualisation(int option);
	void runVisualisation(const Camera& camera, Timecode t);
	void processAudio(const Buffer & buffer);

	std::shared_ptr<PostProcessing> postProcessing() { return m_postProcessing; }

private:
	std::shared_ptr<PostProcessing> m_postProcessing;

	std::vector<Visualisation*> m_visualisations;
	int m_currentVisualisaton;
	bool m_wireframe;

	GLFWwindow* m_window;
};

