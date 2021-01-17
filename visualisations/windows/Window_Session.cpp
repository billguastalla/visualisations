#include "Window_Session.h"
#include "Model_Session.h"
#include <imgui.h>

Window_Session::Window_Session(std::shared_ptr<Model_Session>& session)
	: m_session{session},
	ui_fileName{},
	ui_filePath{},
	ui_title{}
{
}

void Window_Session::draw()
{
	auto strToCharArray = [](const std::string& str, char chr[256]) {
		size_t s{ str.size() };
		for (unsigned i = 0; i < 255u && i < s; ++i)
			chr[i] = str[i];
	};
	strToCharArray(m_session->filename(), ui_fileName);
	strToCharArray(m_session->filepath(), ui_filePath);
	strToCharArray(m_session->title(), ui_title);

	ImGui::Begin(windowTitle().c_str());
	ImGui::Text(std::string{ "Session: [" + m_session->sessionStateString(m_session->state()) + "]" }.c_str());
	ImGui::InputText("title", ui_title, 255);
	ImGui::InputText("file name", ui_fileName, 255);
	ImGui::InputText("file path", ui_filePath, 255);

	std::string fileName{ ui_fileName };
	if (fileName != m_session->filename())
		m_session->setFilename(fileName);
	std::string filePath{ ui_filePath };
	if (filePath != m_session->filepath())
		m_session->setFilepath(filePath);

	if (ImGui::Button("new"))
		m_session->create();
	ImGui::SameLine();
	if (ImGui::Button("open"))
		m_session->open();
	ImGui::SameLine();
	if (ImGui::Button("save"))
		m_session->save();
	ImGui::SameLine();
	if (ImGui::Button("close"))
		m_session->close();

	ImGui::End();
}
