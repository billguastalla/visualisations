#pragma once
#include "UserInterface.h"
#include "Recorder.h"
#include "Visualisation_Oscilloscope.h"
#include "Visualisation_Cubes.h"

class Settings_VideoRendering;
class Settings_AudioInterface;
class Settings_Visualisation;

class Controller_VideoRendering;
class Controller_AudioInterface;
class Controller_Visualisation;

struct GLFWwindow;

class Program
{
public:
	Program(GLFWwindow * window, std::string glslVersion);
	~Program();

	void initialise();
	void deinitialise();

	void run();
private:
	GLFWwindow * m_window;
	std::string m_glslVersion;

	UserInterface m_interface;
	Recorder m_recorder;

	Visualisation_Oscilloscope m_vis_oscilloscope;
	Visualisation_Cubes m_vis_cubes;


	/* Settings Instances */
	std::shared_ptr<Settings_VideoRendering> m_settingsVideoRendering;
	std::shared_ptr<Settings_AudioInterface> m_settingsAudioInterface;
	std::shared_ptr<Settings_Visualisation> m_settingsVisualisation;

	/* Controller Instances */
	std::shared_ptr<Controller_VideoRendering> m_controllerVideoRendering;
	std::shared_ptr<Controller_AudioInterface> m_contollerAudioInterface;
	std::shared_ptr<Controller_Visualisation> m_controllerVisualisation;

};

