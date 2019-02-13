#include "Window_Visualisation.h"

#include "Model_Visualisation.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"

Window_Visualisation::Window_Visualisation(std::shared_ptr<Model_Visualisation> & visualisation)
	: m_visualisation{visualisation}
{
}

Window_Visualisation::~Window_Visualisation()
{
}

void Window_Visualisation::draw()
{
	ImGui::Begin("GFX Statistics:");
	ImGui::Text("\tFramerate: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();


	ImGui::Begin("Visualisation Properties");
	ImGui::Text("Visualisation:\t");
	ImGui::SameLine();

	int sel = m_visualisation->currentVisualisation();
	std::string visOpts = m_visualisation->visualisationOptionsString();
	ImGui::Combo("",&sel, &visOpts[0]);


	ImGui::End();

	m_visualisation->setVisualisation(sel);
}
