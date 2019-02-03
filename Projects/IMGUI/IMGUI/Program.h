#pragma once
#include "UserInterface.h"
#include "Recorder.h"
#include "Visualisation_Oscilloscope.h"

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
};

