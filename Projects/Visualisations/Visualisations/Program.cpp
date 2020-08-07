#include "Program.h"

#include "Settings_VideoRendering.h"
#include "Settings_AudioInterface.h"
#include "Settings_Visualisation.h"

#include "Model_ViewportSystem.h"
#include "Model_VideoRendering.h"
#include "Model_AudioInterface.h"
#include "Model_Visualisation.h"
#include "Model_Transport.h"
#include "Model_Session.h"
#include "PostProcessing.h"

#include "Window_VideoRendering.h"
#include "Window_AudioInterface.h"
#include "Window_Visualisation.h"
#include "Window_ViewportSystem.h"
#include "Window_Transport.h"
#include "Window_Session.h"
#include "Window_PostProcessing.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <imgui/imgui.h>

/* Try to ensure nothing below program level knows about GLFWwindow */
Program::Program(GLFWwindow* window, std::string glslVersion, const ProgramMode& m)
	: m_window{ window },
	m_interface{ this },
	m_glslVersion{ glslVersion },
	m_mode{ m },
	m_postProcessing{ new PostProcessing{} }
{
}

Program::~Program()
{
}

void Program::initialise()
{
	m_interface.initialise(m_window, m_glslVersion.c_str());
	m_postProcessing->initialise();

	/* Set up Model instances */
	m_modelViewportSystem = std::shared_ptr<Model_ViewportSystem>{ new Model_ViewportSystem{m_window} };
	m_modelVideoRendering = std::shared_ptr<Model_VideoRendering>{ new Model_VideoRendering{ this,m_window } };
	m_modelAudioInterface = std::shared_ptr<Model_AudioInterface>{ new Model_AudioInterface{ } };
	m_modelVisualisation = std::shared_ptr<Model_Visualisation>{ new Model_Visualisation{ m_window} };
	m_modelTransport = std::shared_ptr<Model_Transport>{ new Model_Transport{} };
	m_modelSession = std::shared_ptr<Model_Session>{ new Model_Session{*this} };

	/* Set up window instances */
	Window_Abstract* videoRenderWindow = new Window_VideoRendering{ m_modelVideoRendering };
	Window_Abstract* audioInterfaceWindow = new Window_AVIO{ m_modelAudioInterface };
	Window_Abstract* visualisationWindow = new Window_Visualisation{ m_modelVisualisation };
	Window_Abstract* viewportSystemWindow = new Window_ViewportSystem{ m_modelViewportSystem };
	Window_Abstract* transportWindow = new Window_Transport{ m_modelTransport };
	Window_Abstract* sessionWindow = new Window_Session{ m_modelSession };
	Window_Abstract* postProcessingWindow = new Window_PostProcessing{ m_postProcessing };

	m_interface.addWindow(videoRenderWindow);
	m_interface.addWindow(audioInterfaceWindow);
	m_interface.addWindow(visualisationWindow);
	m_interface.addWindow(viewportSystemWindow);
	m_interface.addWindow(transportWindow);
	m_interface.addWindow(sessionWindow);
	m_interface.addWindow(postProcessingWindow);

	glEnable(GL_DEPTH_TEST);
}

void Program::deinitialise()
{
	m_interface.deinitialise();
	m_postProcessing->deinitialise();
}

void Program::run()
{
	// Main loop
	while (!glfwWindowShouldClose(m_window))
	{
		Timestep t{ m_modelTransport->time() };

		if (!m_modelViewportSystem->freeCamera())
			m_modelViewportSystem->processCamera(t); // Update camerapos according to camera system if locked camera.

		const Camera& c{ m_modelViewportSystem->camera() };

		//Gist<float> audioAnalysis{ buf.framecountPerChannel(),test.sampleRate() };
		//audioAnalysis.processAudioFrame(buf.data(0));

		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		/* Later we can pass the interval in samples/ms per frame here*/
		/* Should the Program class care about the check for the record state? */
		if (m_modelAudioInterface->streamRunning())
		{
			Buffer currentAudio = m_modelAudioInterface->buffer();
			m_modelVisualisation->processAudio(currentAudio);
			//m_modelVideoRendering->processAudio();
		}
		interpretMouseInput();
		interpretKeyboardInput();

		m_postProcessing->frameRenderBegin();

		/* Draw the current visualisation. */
		if (m_mode == ProgramMode::Sandbox)
			m_modelVisualisation->runVisualisation(c, t);
		else if (m_mode == ProgramMode::Scripted)
			;// m_modelScene->frame();


		m_postProcessing->frameRenderEnd();
		m_modelVideoRendering->renderFrame();
		m_interface.render();
		//if (m_modelVideoRendering->renderFromMainBuffer())
		//{
		//	m_modelVideoRendering->renderFrame();
		//	m_postProcessing->frameRenderEnd();
		//	m_interface.render();
		//}
		//else
		//{
		//	m_postProcessing->frameRenderEnd();
		//	/* If user wants to see UI in the video output, draw the interface before rendering a video frame. */
		//	if (m_modelVideoRendering->renderUI())
		//	{
		//		m_interface.render();
		//		m_modelVideoRendering->renderFrame();
		//	}
		//	else
		//	{
		//		m_modelVideoRendering->renderFrame();
		//		m_interface.render();
		//	}
		//}


		glfwMakeContextCurrent(m_window);
		glfwSwapBuffers(m_window);

		std::vector<float> backgroundColour{ m_interface.backgroundColour() };
		if (backgroundColour.size() >= 4)
			glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]);
		else
			glClearColor(0.3f, 0.4f, 0.1f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_modelTransport->nextFrame();
	}
}

void Program::setResolution(const std::array<int, 2>& res)
{
	if (res[0] > 16384 || res[1] > 16384 || res[0] < 512 || res[1] < 512)
		return;

	glfwSetWindowSize(m_window, res[0], res[1]);
	m_modelViewportSystem->camera().m_aspectRatio = ((float)res[0] / (float)res[1]);
	int testW{ 0 }, testH{ 0 };
	glfwGetFramebufferSize(m_window, &testW, &testH);
	std::cout << std::endl << "New framebuffer size  w:" << testW << " h:" << testH << std::endl;
}

std::array<int, 2> Program::resolution()
{
	std::array<int, 2> res{};
	glfwGetWindowSize(m_window, &res[0], &res[1]);
	return res;
}

void Program::interpretMouseInput()
{
	if (!ImGui::GetIO().WantCaptureMouse)
	{
		int leftMouse = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1);
		double xPos{ 0.0 }, yPos{ 0.0 };
		glfwGetCursorPos(m_window, &xPos, &yPos);

		m_modelViewportSystem->mouseMovement((float)xPos, (float)yPos, leftMouse == 1);
	}
}

void Program::interpretKeyboardInput()
{
	/* Quickly implement keyboard movement */
	if (!ImGui::GetIO().WantCaptureKeyboard)
	{
		/* Movement */
		int cm{ 0 };
		int w = glfwGetKey(m_window, GLFW_KEY_W);
		int a = glfwGetKey(m_window, GLFW_KEY_A);
		int s = glfwGetKey(m_window, GLFW_KEY_S);
		int d = glfwGetKey(m_window, GLFW_KEY_D);
		int space = glfwGetKey(m_window, GLFW_KEY_SPACE);
		int rightBracket = glfwGetKey(m_window, GLFW_KEY_RIGHT_BRACKET);
		int leftBracket = glfwGetKey(m_window, GLFW_KEY_LEFT_BRACKET);
		int zero = glfwGetKey(m_window, GLFW_KEY_0);
		if (w == GLFW_PRESS)
			cm += (int)Camera_Movement::FORWARD;
		if (a == GLFW_PRESS)
			cm += (int)Camera_Movement::LEFT;
		if (s == GLFW_PRESS)
			cm += (int)Camera_Movement::BACKWARD;
		if (d == GLFW_PRESS)
			cm += (int)Camera_Movement::RIGHT;
		if (space == GLFW_PRESS)
			cm += (int)Camera_Movement::UP;
		if (rightBracket == GLFW_PRESS)
			cm += (int)Camera_Movement::INCREASE_MOVEMENT_SPEED;
		if (leftBracket == GLFW_PRESS)
			cm += (int)Camera_Movement::DECREASE_MOVEMENT_SPEED;
		if (zero == GLFW_PRESS)
			cm += (int)Camera_Movement::RESET_POSITION;
		m_modelViewportSystem->keyMovement((Camera_Movement)cm);
	}
}

