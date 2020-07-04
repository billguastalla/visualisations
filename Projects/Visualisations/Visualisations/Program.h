#pragma once
#include "UserInterface.h"
#include "Recorder.h"

class Model_VideoRendering;
class Model_AudioInterface;
class Model_Visualisation;
class Model_ViewportSystem;
class Model_Transport;
class Model_Session;

struct GLFWwindow;

class Program
{
public:
	enum class ProgramMode { Sandbox, Scripted };

	Program(GLFWwindow* window, std::string glslVersion, const ProgramMode& m = ProgramMode::Sandbox);
	~Program();

	void initialise();
	void deinitialise();

	void run();
private:
	void runSandbox(); // I don't think this is a good method.
	void runScripted();
	void interpretMouseInput();
	void interpretKeyboardInput();

	UserInterface m_interface;

	ProgramMode m_mode;
	GLFWwindow* m_window;
	std::string m_glslVersion;

	// NOTE:	Model is a misnomer really. It's a top-level item large enough to have a separation of
	//			concerns between its functionality and its gui.
	std::shared_ptr<Model_ViewportSystem> m_modelViewportSystem;
	std::shared_ptr<Model_VideoRendering> m_modelVideoRendering;
	std::shared_ptr<Model_AudioInterface> m_modelAudioInterface;
	std::shared_ptr<Model_Visualisation> m_modelVisualisation;
	std::shared_ptr<Model_Transport> m_modelTransport;
	std::shared_ptr<Model_Session> m_modelSession;

};
