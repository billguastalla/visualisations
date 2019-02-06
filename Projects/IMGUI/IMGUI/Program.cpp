#include "Program.h"

#include "Settings_VideoRendering.h"
#include "Settings_AudioInterface.h"
#include "Settings_Visualisation.h"

#include "Controller_VideoRendering.h"
#include "Controller_AudioInterface.h"
#include "Controller_Visualisation.h"

#include "Window_VideoRendering.h"
#include "Window_AudioInterface.h"
#include "Window_Visualisation.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

Program::Program(GLFWwindow * window, std::string glslVersion)
	: m_window{ window },
	m_interface{},
	m_glslVersion{ glslVersion },
	m_vis_oscilloscope{ window },
	m_vis_cubes{ window }
{

}

Program::~Program()
{

}

void Program::initialise()
{
	m_vis_cubes.activate(); /* At some point you'll want to activate/deactivate on switching modes.*/
	m_vis_oscilloscope.activate();

	m_interface.initialise(m_window, m_glslVersion.c_str());
	m_recorder.startMonitoring();


	/* Set up settings instances */
	m_settingsVideoRendering = std::shared_ptr<Settings_VideoRendering>{ new Settings_VideoRendering{} };
	m_settingsAudioInterface = std::shared_ptr<Settings_AudioInterface>{ new Settings_AudioInterface{} };
	m_settingsVisualisation = std::shared_ptr<Settings_Visualisation>{ new Settings_Visualisation{} };

	/* Set up controller instances */
	m_controllerVideoRendering = std::shared_ptr<Controller_VideoRendering>{ new Controller_VideoRendering{ m_settingsVideoRendering } };
	m_contollerAudioInterface = std::shared_ptr<Controller_AudioInterface>{ new Controller_AudioInterface{ m_settingsAudioInterface } };
	m_controllerVisualisation = std::shared_ptr<Controller_Visualisation>{ new Controller_Visualisation{ m_settingsVisualisation } };

	/* Set up window instances */
	Window_Abstract * videoRenderWindow = new Window_VideoRendering{ m_controllerVideoRendering };
	Window_Abstract * audioInterfaceWindow = new Window_AudioInterface{ m_contollerAudioInterface };
	Window_Abstract * visualisationWindow = new Window_Visualisation{ m_controllerVisualisation };

	m_interface.addWindow(videoRenderWindow);
	m_interface.addWindow(audioInterfaceWindow);
	m_interface.addWindow(visualisationWindow);


	glEnable(GL_DEPTH_TEST);
}

void Program::deinitialise()
{
	m_vis_cubes.deactivate();
	m_vis_oscilloscope.deactivate();

	m_interface.deinitialise();
	m_recorder.stopMonitoring();
}

void Program::run()
{
	// Main loop
	while (!glfwWindowShouldClose(m_window))
	{
		Buffer audioBuffer = m_recorder.getBuffer();


		m_interface.render();

		//Gist<float> audioAnalysis{ buf.framecountPerChannel(),test.sampleRate() };
		//audioAnalysis.processAudioFrame(buf.data(0));

		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

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
		glClearColor(0.3f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


}
