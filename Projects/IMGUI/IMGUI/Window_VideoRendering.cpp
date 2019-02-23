#include "Window_VideoRendering.h"

#include "Model_VideoRendering.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"

#include <vector>
#include <string>

Window_VideoRendering::Window_VideoRendering(std::shared_ptr<Model_VideoRendering> & Model)
	:
	m_videoModel{ Model }
{
}

Window_VideoRendering::~Window_VideoRendering()
{
}

void Window_VideoRendering::draw()
{
	ImGui::Begin("Video Rendering");

	/*	Should include:
		-> Output file name.
		-> Duration Mode (Set by frames, set by samples, set by time, click to stop)
		-> Checkbox: Render UI enabled/disabled
		-> Codec selection
		-> File format selection
		-> Checkbox: Record Audio
		-> Buttons: Record/Pause & Stop. (held by Model and not settings?)
	*/
	std::string fn = m_videoModel->fileName();

	char fnChar[255];
	memcpy(fnChar, fn.c_str(), fn.size());
	for (int c = fn.size(); c < 255; ++c)
		fnChar[c] = '\0';

	ImGui::Text("Output Filename");
	ImGui::SameLine();
	ImGui::InputText("", fnChar, 255);

	//bool recAudio{ m_videoModel->recordAudio() };
	//ImGui::Checkbox("Record Audio", &recAudio);
	//m_videoModel->setRecordAudio(recAudio);

	//bool renderUI{ m_videoModel->renderUI() };
	//ImGui::Checkbox("Render UI", &renderUI);
	//m_videoModel->setRenderUI(renderUI);


	ImGui::Text("Transport: ");
	ImGui::SameLine();

	bool pauseClicked{ false }, startClicked{ false }, resumeClicked{ false }, stopClicked{ false };
	switch (m_videoModel->state())
	{
	case Model_VideoRendering::RecordState::Started:
		pauseClicked = ImGui::Button("Pause");
		ImGui::SameLine();
		stopClicked = ImGui::Button("Stop");
		break;
	case Model_VideoRendering::RecordState::Paused:
		resumeClicked = ImGui::Button("Resume");
		ImGui::SameLine();
		stopClicked = ImGui::Button("Stop");
		break;
	case Model_VideoRendering::RecordState::Stopped:
		startClicked = ImGui::Button("Start");
		ImGui::SameLine();
		stopClicked = ImGui::Button("Stop");
		break;
	default:
		break;
	}
	if (pauseClicked)
		m_videoModel->pause();
	else if (startClicked)
		m_videoModel->start();
	else if (stopClicked)
		m_videoModel->stop();
	else if (resumeClicked)
		m_videoModel->pause();


	//fn = std::string{ fnChar };
	fn.clear();
	for (int c = 0; c < 255; ++c)
		if (fnChar[c] != '\0')
			fn.push_back(fnChar[c]);
		else
			break;
	m_videoModel->setFileName(fn);

	ImGui::End();
}
