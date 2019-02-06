#include "Window_VideoRendering.h"

#include "Controller_VideoRendering.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"

#include <string>

Window_VideoRendering::Window_VideoRendering(std::shared_ptr<Controller_VideoRendering> & controller)
	:
	m_videoController{controller}
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
		-> Buttons: Record/Pause & Stop. (held by controller and not settings?)
	*/
	char fileName[255];
	ImGui::InputText("", fileName, 255);


	ImGui::End();
}
