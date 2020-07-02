#pragma once
#include "UserInterface.h"
#include "Recorder.h"

class Settings_VideoRendering;
class Settings_AudioInterface;
class Settings_Visualisation;

class Model_VideoRendering;
class Model_AudioInterface;
class Model_Visualisation;
class Model_ViewportSystem;

struct GLFWwindow;

typedef double Timecode;

class Program
{
public:
	enum class ProgramMode { Sandbox, Scripted };

	Program(GLFWwindow * window, std::string glslVersion, const ProgramMode & m = ProgramMode::Scripted);
	~Program();

	void initialise();
	void deinitialise();

	void run(const ProgramMode & m);

	/* This one way to do it, not very thought through yet. */
//	void updateGlobalAudioBuffer(std::shared_ptr<LockableBuffer> & buf);
private:
	ProgramMode m_mode;

	void interpretMouseInput();
	void interpretKeyboardInput();

	GLFWwindow * m_window;
	std::string m_glslVersion;

	UserInterface m_interface;

	/* Model Instances */
	std::shared_ptr<Model_ViewportSystem> m_modelViewportSystem;
	std::shared_ptr<Model_VideoRendering> m_modelVideoRendering;
	std::shared_ptr<Model_AudioInterface> m_modelAudioInterface;
	std::shared_ptr<Model_Visualisation> m_modelVisualisation;
};

