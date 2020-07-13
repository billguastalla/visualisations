#pragma once
#include <string>

#include <vector>
#include <memory>

class Window_Abstract;

struct GLFWwindow;
class Buffer;
class Program;

class UserInterface
{
public:
	UserInterface(Program * prog);
	~UserInterface();

	void initialise(GLFWwindow * window, std::string glslVersion);
	void deinitialise();

	/* Passed window will now be owned & managed by the UserInterface instance.
		So do not destroy it, and do not keep this pointer outside. */
	void addWindow(Window_Abstract * win);
	void render();

	std::vector<float> backgroundColour() { return m_clearColour; }
private:
	std::vector<std::unique_ptr<Window_Abstract>> m_windows;

	bool m_showMainWindow;
	std::vector<float> m_clearColour;

	int ui_programMode; // NOTE: type is Program::ProgramMode
	Program* p_program;
};

