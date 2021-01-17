#include "Window_Visualisation.h"
#include "../models/Model_Visualisation.h"

#include <imgui.h>

Window_Visualisation::Window_Visualisation(std::shared_ptr<Model_Visualisation> & visualisation)
	: m_visualisation{visualisation}
{
}

Window_Visualisation::~Window_Visualisation()
{
}

void Window_Visualisation::draw()
{
	ImGui::Begin("Visualisation Properties");
	ImGui::Text("Visualisation:\t");
	ImGui::SameLine();

	int sel = m_visualisation->currentVisualisationIndex();
	std::string visOpts = m_visualisation->visualisationOptionsString();
	ImGui::Combo("",&sel, &visOpts[0]);

	std::string s{ "Options: " };
	s += m_visualisation->currentVisualisation()->titleString();
	s += ":";
	ImGui::Text(s.c_str());
	m_visualisation->currentVisualisation()->drawInterface();
	

	ImGui::End();

	
	m_visualisation->setVisualisation(sel);
}
