#pragma once
#include <string>

#include <vector>
#include <memory>

class Window_Abstract;

struct GLFWwindow;
class Buffer;

class UserInterface
{
public:
	UserInterface();
	~UserInterface();

	void initialise(GLFWwindow * window, std::string glslVersion);
	void deinitialise();

	/* Passed window will now be owned & managed by the UserInterface instance.
		So do not destroy it, and do not keep this pointer outside. */
	void addWindow(Window_Abstract * win);
	void render();

	//void hideAll();
	//void showAll();

private:
	std::vector<std::unique_ptr<Window_Abstract>> m_windows;
	bool m_showMainWindow;
};

