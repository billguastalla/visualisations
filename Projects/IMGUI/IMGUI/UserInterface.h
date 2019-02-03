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
private:

};

