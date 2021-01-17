#include "UserInterface.h"
#include "../audio/Buffer.h"
#include "../general/About.h"
#include "../windows/Window_Abstract.h"
#include "../program/Program.h"

#include <glad\glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

// WARNING: state is connected to Program::ProgramMode
constexpr char programModeOptsString[] = "Sandbox\0Scripted\0";

UserInterface::UserInterface(Program * prog)
	:
	p_program{prog}, // Warning: pointer passed before construction of program finished.
	ui_programMode{(int)Program::ProgramMode::Sandbox},
	m_showMainWindow{ true },
	m_windows{},
	m_aboutDlg{false},
	m_clearColour{ {0.11f,0.24f,0.35f,1.0f} }
{
}

UserInterface::~UserInterface()
{
}

void UserInterface::initialise(GLFWwindow * window, std::string glslVersion)
{
	// Setup ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glslVersion.c_str());

	// Setup style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ui_winSize = p_program->resolution();
}

void UserInterface::deinitialise()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UserInterface::addWindow(Window_Abstract * win)
{
	m_windows.push_back(std::unique_ptr<Window_Abstract>{win});
}

void UserInterface::render()
{
	// IMGUI Render Start Calls
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Draw visible windows
	for (std::vector<std::unique_ptr<Window_Abstract>>::iterator i = m_windows.begin(); i != m_windows.end(); ++i)
		if ((*i)->visible())
			(*i)->draw();

	// Draw the main window
	if (m_showMainWindow)
	{
		ImGui::Begin("Interface");

		int progMode{ ui_programMode };
		ImGui::Combo("Program Mode", &progMode, programModeOptsString);
		
		ImGui::Text("Windows:\t");
		ImGui::Checkbox("Main Window (toggle with \"i\")", &m_showMainWindow);
		for (std::vector<std::unique_ptr<Window_Abstract>>::iterator i = m_windows.begin(); i != m_windows.end(); ++i)
		{
			ImGui::Checkbox((*i)->windowTitle().c_str(), &(*i)->visible());
		}
		ImGui::Text("Properties:\t");
		ImGui::SliderFloat4("Background Colour", &m_clearColour[0], 0.0f, 1.0f);

		ImGui::Text(std::string{ "Current resolution: {" + std::to_string(p_program->resolution()[0]) + ", "
						+ std::to_string(p_program->resolution()[1]) + "}" }.c_str());
		ImGui::SliderInt2("Change resolution",&ui_winSize[0],512,8192);
		ImGui::SameLine();
		if (ImGui::Button("Apply"))
			p_program->setResolution(ui_winSize);


		ImGui::Text("Key Bindings:\t");
		ImGui::Text("\tMove:\t W,A,S,D");
		ImGui::Text("\tLook around:\t Left-click");
		ImGui::Text("\tSpeed up/down:\t [ / ]");
		ImGui::Text("\tReset Position:\t 0");


		ImGui::Checkbox("About", &m_aboutDlg);
		if(m_aboutDlg)
			drawAboutDialog();

		ImGui::End();

		if (ui_programMode != progMode && p_program != nullptr)
		{
			p_program->setMode((Program::ProgramMode)progMode);
			ui_programMode = progMode;
		}
	}
	// Press I to toggle main window 
	if (ImGui::IsKeyPressed('I'))
		m_showMainWindow = !m_showMainWindow;


	// IMGUI Render Finish Calls
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
