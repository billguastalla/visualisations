#include "About.h"
#include <imgui/imgui.h>

void drawAboutDialog()
{
	ImVec2 midpoint{ ImGui::GetIO().DisplaySize };
	midpoint.x = midpoint.x / 2.;
	midpoint.y = midpoint.y / 2.;
	ImVec2 dialogSize{ 200,200 };

	ImGui::SetNextWindowSize(dialogSize, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(midpoint, ImGuiCond_FirstUseEver);

	ImGui::Begin("About");
	ImGui::Text(about.c_str());
	ImGui::End();
}
