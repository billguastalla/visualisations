#include "Window_AudioInterface.h"

#include <imgui.h>
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "../models/Model_AudioInterface.h"

Window_AVIO::Window_AVIO(std::shared_ptr<Model_AudioInterface> & audioInterface)
	: m_audioInterface{ audioInterface },
	m_recorderUI{ new RecorderUI{audioInterface->recorder()} }
{
}

Window_AVIO::~Window_AVIO()
{
}

void Window_AVIO::draw()
{
	ImGui::Begin(windowTitle().c_str());

	static float f = 0.0f;

	m_recorderUI->draw();


	ImGui::Text("Audio Statistics:");
	
	//ImGui::Text("\tChannels: %i", audioBuffer.channelCount());
	//ImGui::Text("\tAmplitude Average: %.3f", audioBuffer.amplitude_average());
	//ImGui::Text("\tAmplitude Minimum: %.3f", audioBuffer.amplitude_minimum());
	//ImGui::Text("\tAmplitude Peak: %.3f", audioBuffer.amplitude_peak());


	ImGui::End();

}
