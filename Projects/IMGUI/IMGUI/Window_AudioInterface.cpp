#include "Window_AudioInterface.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"

#include "Model_AudioInterface.h"
#include "Settings_AudioInterface.h"

Window_AudioInterface::Window_AudioInterface(std::shared_ptr<Model_AudioInterface> & audioInterface)
	: m_audioInterface{ audioInterface }
{
}


Window_AudioInterface::~Window_AudioInterface()
{
}

void Window_AudioInterface::draw()
{
	static float f = 0.0f;
	ImGui::Begin("Audio Properties");
	ImGui::Text("Audio Statistics:");
	//ImGui::Text("\tChannels: %i", audioBuffer.channelCount());
	//ImGui::Text("\tAmplitude Average: %.3f", audioBuffer.amplitude_average());
	//ImGui::Text("\tAmplitude Minimum: %.3f", audioBuffer.amplitude_minimum());
	//ImGui::Text("\tAmplitude Peak: %.3f", audioBuffer.amplitude_peak());
	ImGui::End();

	std::shared_ptr<Settings_AudioInterface> settings =  m_audioInterface->settings();
}
