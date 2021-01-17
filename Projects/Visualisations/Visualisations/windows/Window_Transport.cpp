#include "Window_Transport.h"
#include "../models/Model_Transport.h"
#include <imgui.h>

void Window_Transport::draw()
{
	ImGui::Begin(windowTitle().c_str());

	ImGui::Text(std::string{"Transport State: [" + m_transport->stateStr() + "]"}.c_str());
	ImGui::SameLine();
	if (ImGui::Button("switch state"))
		m_transport->setTransportState((TransportState)!(int)m_transport->state()); // oops

	ImGui::Text(std::string{ "Time Mode: [" + m_transport->modeStr() + "]" }.c_str());
	ImGui::SameLine();
	if (ImGui::Button("switch mode"))
		m_transport->setTimeMode((TimeMode)!(int)m_transport->mode()); // ...

	int currentFrame{ m_transport->currentFrame() };
	int maxFrames{ m_transport->totalFrames() };
	int framerate{ m_transport->framerate() };
	ImGui::SliderInt("Transport",&currentFrame,0,maxFrames);

	ImGui::Text(std::string{ "Time: {" + std::to_string(m_transport->time().current().count()) + "s}" }.c_str());
	ImGui::InputInt("Total Frames", &maxFrames, 1);
	ImGui::InputInt("Framerate",&framerate,15);

	if (currentFrame != m_transport->currentFrame())
		m_transport->setCurrentFrame(currentFrame);
	if (maxFrames != m_transport->totalFrames())
	{
		m_transport->setTotalFrames(maxFrames);
		if (maxFrames > currentFrame)
			m_transport->setCurrentFrame(maxFrames);
	}
	if (framerate != m_transport->framerate() && framerate > 0)
		m_transport->setFrameRate(framerate);

	ImGui::End();
}
