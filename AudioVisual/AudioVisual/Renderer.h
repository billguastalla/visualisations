#pragma once
#include "Vertex.h"

class Shader;
struct GLFWwindow;
class Recorder;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void renderLoop(const Recorder & recorder);

private:
	Shader * m_shader;
	GLFWwindow * m_window;
	unsigned int m_VBO; /* This is a handle. */
	unsigned int m_VAO;

};