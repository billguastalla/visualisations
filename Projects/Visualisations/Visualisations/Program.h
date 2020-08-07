#pragma once
#include "UserInterface.h"
#include "Recorder.h"

class Model_VideoRendering;
class Model_AudioInterface;
class Model_Visualisation;
class Model_ViewportSystem;
class Model_Transport;
class Model_Session;
class PostProcessing;

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

	// note:	these functions exist to provide other models to session via Program*. 
	//			open to an alternative.
	std::shared_ptr<Model_ViewportSystem> modelViewportSystem() const { return m_modelViewportSystem; }
	std::shared_ptr<Model_VideoRendering> modelVideoRendering() const { return m_modelVideoRendering; }
	std::shared_ptr<Model_AudioInterface> modelAudioInterface() const { return m_modelAudioInterface; }
	std::shared_ptr<Model_Visualisation> modelVisualisation() const { return m_modelVisualisation; }
	std::shared_ptr<Model_Transport> modelTransport() const { return m_modelTransport; }
	std::shared_ptr<Model_Session> modelSession() const { return m_modelSession; }

	std::shared_ptr<PostProcessing> postProcessing() const { return m_postProcessing; }

	void setMode(const ProgramMode& mode) { m_mode = mode; }
	ProgramMode mode() { return m_mode; }

	void setResolution(const std::array<int,2> & res);
	std::array<int, 2> resolution();
private:
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

	std::shared_ptr<PostProcessing> m_postProcessing;
};
