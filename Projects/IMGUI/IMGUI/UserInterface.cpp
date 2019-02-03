#include "UserInterface.h"
#include "Buffer.h"

#include <glad\glad.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"

UserInterface::UserInterface()
	:
	m_visItems{"Oscilloscope\0Cubes\0"},
	m_visSelection{0}
{
}

UserInterface::~UserInterface()
{
}

void UserInterface::initialise(GLFWwindow * window, std::string glslVersion)
{
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glslVersion.c_str());

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

void UserInterface::deinitialise()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UserInterface::render(const Buffer & audioBuffer)
{
	// IMGUI Render Start Calls
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	static float f = 0.0f;
	ImGui::Begin("Audio Properties");
	ImGui::Text("Audio Statistics:");
	ImGui::Text("\tChannels: %i", audioBuffer.channelCount());
	ImGui::Text("\tAmplitude Average: %.3f", audioBuffer.amplitude_average());
	ImGui::Text("\tAmplitude Minimum: %.3f", audioBuffer.amplitude_minimum());
	ImGui::Text("\tAmplitude Peak: %.3f", audioBuffer.amplitude_peak());
	ImGui::End();

	ImGui::Begin("GFX Statistics:");
	ImGui::Text("\tFramerate: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();


	ImGui::Begin("Visualisation Properties");
	ImGui::Text("Visualisation:\t");
	ImGui::SameLine();
	ImGui::Combo("",&m_visSelection, m_visItems);
	ImGui::End();

	ImGui::Begin("Renderer");
	ImGui::End();

	//ImGui::Text("Audio Settings:");
	//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//ImGui::Checkbox("Another Window", &show_another_window);
	//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
	//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
	//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//	counter++;
	//ImGui::SameLine();
	//ImGui::Text("counter = %d", counter);


	// IMGUI Render Finish Calls
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
