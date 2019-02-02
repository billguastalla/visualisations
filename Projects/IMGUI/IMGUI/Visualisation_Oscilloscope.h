#pragma once
#include "Vertex.h"

class Shader;
struct GLFWwindow;
class Buffer;

class Visualisation_Oscilloscope
{
public:
	Visualisation_Oscilloscope(GLFWwindow * win);
	~Visualisation_Oscilloscope();

	void drawVisualisations(const Buffer & buf);

private:
	Shader * m_shader;
	GLFWwindow * m_window;
	unsigned int m_VBO; /* This is a handle. */
	unsigned int m_VAO;

};