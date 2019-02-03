#pragma once
#include <string>

struct GLFWwindow;
class Buffer;

class UserInterface
{
public:
	UserInterface();
	~UserInterface();

	void initialise(GLFWwindow * window, std::string glslVersion);
	void deinitialise();

	void render(const Buffer & audioBuffer);

	int visualisationSelection() { /*Temp: Need to move this part of UI to a visualisation window, that is itself connected to either
												the program or some concrete visualisation handling entity .*/
		return m_visSelection;
	}
private:
	int m_visSelection;
	const char * m_visItems;
};

