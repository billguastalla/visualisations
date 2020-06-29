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
	ImGui::Begin("Visualisation Properties");
	ImGui::Text("Visualisation:\t");
	ImGui::SameLine();

	int sel = m_visualisation->currentVisualisationIndex();
	std::string visOpts = m_visualisation->visualisationOptionsString();
	ImGui::Combo("",&sel, &visOpts[0]);

	glm::vec3 cPos = m_visualisation->currentVisualisation()->cameraPosition();
	ImGui::Text("\tFramerate: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text(std::string{"Camera Position = {" +
		std::to_string(cPos.x).substr(0,5) + ", " +
		std::to_string(cPos.y).substr(0,5) + ", " +
		std::to_string(cPos.z).substr(0,5) + "} "}.c_str());

	ImGui::Text("Post Processing:");
	/* Post-processing details: */
	bool wireframe = m_visualisation->postProcessing()->wireframe();
	ImGui::Checkbox("Wireframe", &wireframe);
	m_visualisation->postProcessing()->setWireframe(wireframe);

	bool hdrEnabled = m_visualisation->postProcessing()->HDREnabled();
	ImGui::Checkbox("HDR", &hdrEnabled);
	m_visualisation->postProcessing()->setHDREnabled(hdrEnabled);

	float exposure = m_visualisation->postProcessing()->exposure();
	ImGui::Text("Exposure");
	ImGui::SameLine();
	ImGui::SliderFloat("", &exposure, 0.0, 1.0);
	m_visualisation->postProcessing()->setExposure(exposure);

	/* Strange post-processing */
	glm::vec2 colBufPerc = m_visualisation->postProcessing()->colBufPerc();
	glm::vec2 colBufSlide{ colBufPerc };
	ImGui::SliderFloat2("Colour Buffer Offset", (float*)&colBufPerc, 0.9f, 1.0f);
	glm::vec2 rbPerc = m_visualisation->postProcessing()->rbPerc();
	glm::vec2 rbSlide{ rbPerc };
	ImGui::SliderFloat2("Render Buffer Offset", (float*)& rbPerc, 0.9f, 1.0f);
	glm::vec2 pingPongPerc = m_visualisation->postProcessing()->pingPongPerc();
	glm::vec2 pingPongSlide{ pingPongPerc };
	ImGui::SliderFloat2("Ping Pong Offset", (float*)& pingPongPerc, 0.9f, 1.0f);
	if (colBufPerc != colBufSlide)
		m_visualisation->postProcessing()->setColBufPerc(colBufPerc);
	if (rbPerc != rbSlide)
		m_visualisation->postProcessing()->setrbPerc(rbPerc);
	if (pingPongPerc != pingPongSlide)
		m_visualisation->postProcessing()->setpingpongPerc(pingPongPerc);




	std::string s{ "Options: " };
	s += m_visualisation->currentVisualisation()->titleString();
	s += ":";
	ImGui::Text(s.c_str());
	m_visualisation->currentVisualisation()->drawInterface();
	

	ImGui::End();

	
	m_visualisation->setVisualisation(sel);
}
