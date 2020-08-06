#pragma once
#include <array>
#include <glad/glad.h>

class Shader;

// This is used to allow resampling, and for rendering to a higher resolution than shown on-screen.
class FrameBufferSystem
{
public:
	FrameBufferSystem();

	void activate();

	void startFrame();
	void endFrame();
	void setResolution(const std::array<int, 2>& res);
	std::array<int,2> resolution();
private:
	GLuint m_frameBufferHandle;
	Shader* m_frameBufferShader;
	GLuint m_textureHandle;

	std::array<int, 2> m_frameBufferResolution;
};