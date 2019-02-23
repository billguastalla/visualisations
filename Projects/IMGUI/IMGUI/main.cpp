#include <glad\glad.h>
#include <GLFW/glfw3.h>

#include "Program.h"

#include <string>
#include <stdio.h>

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
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

	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
		return 1;

	// GL 3.0 + GLSL 130
	std::string glsl_version{ "#version 330" };
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); /* 4.5 is our latest version, reduce this for public release. */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
	glfwWindowHint(GLFW_SAMPLES, 4); /* MSAA */

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Visualisations", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync (Note: Disable if you want to test performance)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		return -1;
	}
	glEnable(GL_MULTISAMPLE); /* MSAA */

	Program program{ window, glsl_version};
	program.initialise();
	program.run();
	program.deinitialise();
	
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
