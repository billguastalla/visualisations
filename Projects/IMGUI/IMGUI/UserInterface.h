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

	//int visualisationSelection() { /*Temp: Need to move this part of UI to a visualisation window, that is itself connected to either
	//											the program or some concrete visualisation handling entity .*/
	//	return m_visSelection;
	//}
private:
	//int m_visSelection;
	//const char * m_visItems;

	std::vector<std::unique_ptr<Window_Abstract>> m_windows;
};

