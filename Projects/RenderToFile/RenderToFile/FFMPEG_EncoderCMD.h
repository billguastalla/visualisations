#pragma once
#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <fstream>

class FFMPEG_EncoderCMD
{
public:
	FFMPEG_EncoderCMD(unsigned int width, unsigned int height);

	void start();
	void renderFrame();
	void stop();
private:
	bool m_started;
	unsigned int m_width, m_height;
	int * m_buffer;
	FILE * m_ffmpeg;
};