#include "Program.h"

#include "Settings_VideoRendering.h"
#include "Settings_AudioInterface.h"
#include "Settings_Visualisation.h"

#include "Model_VideoRendering.h"
#include "Model_AudioInterface.h"
#include "Model_Visualisation.h"

#include "Window_VideoRendering.h"
#include "Window_AudioInterface.h"
#include "Window_Visualisation.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <imgui/imgui.h>

Program::Program(GLFWwindow * window, std::string glslVersion)
	: m_window{ window },
	m_interface{},
	m_glslVersion{ glslVersion }
{

}

Program::~Program()
{

}

void Program::initialise()
{
	m_interface.initialise(m_window, m_glslVersion.c_str());

	/* Set up settings instances */
	m_settingsVideoRendering = std::shared_ptr<Settings_VideoRendering>{ new Settings_VideoRendering{} };
	m_settingsAudioInterface = std::shared_ptr<Settings_AudioInterface>{ new Settings_AudioInterface{} };
	m_settingsVisualisation = std::shared_ptr<Settings_Visualisation>{ new Settings_Visualisation{} };

	/* Set up Model instances */
	m_modelVideoRendering = std::shared_ptr<Model_VideoRendering>{ new Model_VideoRendering{ m_settingsVideoRendering } };
	m_modelAudioInterface = std::shared_ptr<Model_AudioInterface>{ new Model_AudioInterface{ m_settingsAudioInterface } };
	m_modelVisualisation = std::shared_ptr<Model_Visualisation>{ new Model_Visualisation{ m_settingsVisualisation } };

	/* Set up window instances */
	Window_Abstract * videoRenderWindow = new Window_VideoRendering{ m_modelVideoRendering };
	Window_Abstract * audioInterfaceWindow = new Window_AudioInterface{ m_modelAudioInterface };
	Window_Abstract * visualisationWindow = new Window_Visualisation{ m_modelVisualisation };

	m_interface.addWindow(videoRenderWindow);
	m_interface.addWindow(audioInterfaceWindow);
	m_interface.addWindow(visualisationWindow);


	glEnable(GL_DEPTH_TEST);
}

void Program::deinitialise()
{

	m_interface.deinitialise();
}

void Program::run()
{
	// Main loop
	while (!glfwWindowShouldClose(m_window))
	{
		m_interface.render();

		//Gist<float> audioAnalysis{ buf.framecountPerChannel(),test.sampleRate() };
		//audioAnalysis.processAudioFrame(buf.data(0));

		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		/* Later we can pass the interval in samples/ms per frame here*/
		if (m_modelAudioInterface->state() == Model_AudioInterface::RecordState::Started)
		{
			Buffer currentAudio = m_modelAudioInterface->buffer();
			m_modelVisualisation->processAudio(currentAudio);
			//m_modelVideoRendering->processAudio();
		}


		if (! ImGui::GetIO().WantCaptureMouse)
		{
			int leftMouse = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1);
			double xPos{ 0.0 }, yPos{ 0.0 };
			glfwGetCursorPos(m_window, &xPos, &yPos);

			m_modelVisualisation->currentVisualisation()->mouseMovement(xPos,yPos,leftMouse == 1);

			//m_modelVisualisation->currentVisualisation()->camera().ProcessMouseMovement();
		}

		
		m_modelVisualisation->runVisualisation();
		m_modelVideoRendering->renderFrame();


		//int visualisation = m_interface.visualisationSelection();
		//if (visualisation == 0)
		//{
		//	m_vis_oscilloscope.processSamples(audioBuffer, 20);
		//	m_vis_oscilloscope.renderFrame();
		//}
		//else if (visualisation == 1)
		//{
		//	m_vis_cubes.processSamples(audioBuffer, 20);
		//	m_vis_cubes.renderFrame();
		//}

		/*
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);*/

		glfwMakeContextCurrent(m_window);
		glfwSwapBuffers(m_window);

		std::vector<float> backgroundColour = m_interface.backgroundColour();
		if (backgroundColour.size() >= 4)
			glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]);
		else
			glClearColor(0.3f, 0.4f, 0.1f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


}

//void Program::updateGlobalAudioBuffer(std::shared_ptr<LockableBuffer>& buf)
//{
//	m_modelVisualisation->setBuffer(buf);
//}