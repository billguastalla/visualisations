#pragma once
#include "Visualisation.h"
#include "PostProcessing.h"
#include <boost/property_tree/ptree_fwd.hpp>
#include "Timekeeping.h"

#include <memory>
#include <vector>
#include <boost/serialization/access.hpp>

class Settings_Visualisation;
class LockableBuffer;
struct GLFWwindow;

class Model_Visualisation
{
public:
	bool loadFileTree(const boost::property_tree::ptree& t);
	bool saveFileTree(boost::property_tree::ptree& t) const;

	Model_Visualisation(GLFWwindow * win);
	~Model_Visualisation();

	int visualisationOptionsCount();
	int currentVisualisationIndex() { return m_currentVisualisaton; }
	Visualisation* currentVisualisation() { return (m_currentVisualisaton != -1) ? m_visualisations[m_currentVisualisaton] : nullptr; }
	std::string visualisationOptionsString();
	void setVisualisation(int option);
	void runVisualisation(const Camera& camera, Timestep t);
	void processAudio(const Buffer & buffer);

private:

	std::vector<Visualisation*> m_visualisations;
	int m_currentVisualisaton;
	bool m_wireframe;

	GLFWwindow* m_window;

public:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& m_currentVisualisaton;
		ar& m_wireframe;
	}

};

