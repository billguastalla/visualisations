#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "general/About.h"
#include "program/Program.h"

#include <string>
#include <stdio.h>
#include <iostream>

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static int mainInit(GLFWwindow * & window, int width, int height)
{
	// TODO: Platform-specific get for window resolution, pass it into screen.
	glfwSetErrorCallback(glfw_error_callback);

	// true == 1, false == 0
	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); /* 4.5 is our latest version, reduce this for public release. */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 8); /* MSAA */

	// Create window with graphics context
	window = glfwCreateWindow(1920, 1080, "Visualisations 0.4 {(c) Bill Guastalla 2020}", nullptr, nullptr);
	if (window == nullptr)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // vsync
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		return -1;
	}
	glEnable(GL_MULTISAMPLE); /* MSAA */
	return 0;
}

static void mainDeinit(GLFWwindow * window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

int main(int, char**)
{
	/* Note: As per https://stackoverflow.com/questions/5995433/removing-console-window-for-glut-freeglut-glfw ,
			-> The best solution to console is to change to WinMain() as an entry point.
			-> For now we'll just use FreeConsole()
	*/
#ifdef WIN32
	//FreeConsole();
#endif
	std::cout << about;


	GLFWwindow* window{ nullptr };
	mainInit(window, 1920,1080);

	std::string glsl_version{ "#version 330" };
	Program program{ window, glsl_version};
	program.initialise();
	program.run();
	program.deinitialise();

	mainDeinit(window);

	return 0;
}
