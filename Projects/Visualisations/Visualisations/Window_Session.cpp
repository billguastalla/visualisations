#include "Window_Session.h"
#include "Model_Session.h"
#include <imgui/imgui.h>

Window_Session::Window_Session(std::shared_ptr<Model_Session>& session)
	: m_session{session}
{
}

void Window_Session::draw()
{
	auto strToCharArray = [](const std::string& str, char chr[256]) {
		for (unsigned i = 0; i < 255u && i < str.size(); ++i)
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

	ImGui::Button("open");
	ImGui::SameLine();
	if (ImGui::Button("save"))
		m_session->save();

	ImGui::End();
}
