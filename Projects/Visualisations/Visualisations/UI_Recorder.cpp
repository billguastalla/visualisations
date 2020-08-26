#include "UI_Recorder.h"
#include <imgui/imgui.h>

void RecorderUI::draw() const
{
	Recorder::RecorderState state = m_recorder->recorderState();
	bool startClicked{ false }, stopClicked{ false };
	if (state != Recorder::RecorderState::Monitoring)
	{
		startClicked = ImGui::Button("Start");
	}
	else
		stopClicked = ImGui::Button("Stop");

	if (startClicked)
		m_recorder->startMonitoring();
	if (stopClicked)
		m_recorder->stopMonitoring();

	std::string status{ "Status: [" };
	status += m_recorder->recorderStateStr();
	status += "]";
	ImGui::Text(status.c_str());

	int channels{ m_recorder->channelCount() };
	int prevChannels{ channels };
	ImGui::SliderInt("Channels", &channels, 1, 2);
	int framesPerPacket = m_recorder->framesPerPacket();
	int prevFramesPerPacket{ framesPerPacket };
	ImGui::SliderInt("Frames Per Packet", &framesPerPacket, 64, 2048);
	int packetsPerBuffer = m_recorder->packetsPerBuffer();
	int prevPacketsPerBuffer{ packetsPerBuffer };
	ImGui::SliderInt("Packets Per Buffer", &packetsPerBuffer, 8, 256);
	if (channels != prevChannels)
		m_recorder->setChannelCount(channels);
	if (framesPerPacket != prevFramesPerPacket)
		m_recorder->setFramesPerPacket(framesPerPacket);
	if (packetsPerBuffer != prevPacketsPerBuffer)
		m_recorder->setPacketsPerBuffer(packetsPerBuffer);
}
