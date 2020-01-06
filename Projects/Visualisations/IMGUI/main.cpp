#include <glad\glad.h>
#include <GLFW/glfw3.h>

#include "Program.h"

#include <string>
#include <stdio.h>

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static int mainInit(GLFWwindow * & window, int width, int height)
{
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); /* 4.5 is our latest version, reduce this for public release. */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4); /* MSAA */

	// Create window with graphics context
	window = glfwCreateWindow(1920, 1080, "Visualisations", nullptr, nullptr);
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
