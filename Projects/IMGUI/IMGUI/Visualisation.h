#pragma once
#include <string>
#include "Camera.h"

class Buffer;
class Visualisation
{
public:
	Visualisation() :
		m_active{ false },
		m_camera{ glm::vec3(0.0f, 0.0f, 3.0f) },
		m_deltaTime{},
		m_mouseDown{ false },
		m_lastFrame{ 0.0f },
		m_lastX{ 0.0f },
		m_lastY{ 0.0f }
	{};

	/* Set up shaders, initial VAOs, VBOs */
	virtual void activate() = 0;
	/* Unload shaders etc. */
	virtual void deactivate() = 0;

	virtual void processSamples(const Buffer& buf, unsigned samples) = 0;
	virtual void renderFrame() = 0;

	virtual std::string titleString() = 0;


	void mouseMovement(float xPos, float yPos, bool mouseDown)
	{
		if (mouseDown)
		{
			if (m_firstMouse)
			{
				m_lastX = xPos;
				m_lastY = yPos;
				m_firstMouse = false;
			}
			else
			{
				float xoffset = xPos - m_lastX;
				float yoffset = m_lastY - yPos;
				m_lastX = xPos;
				m_lastY = yPos;
				m_camera.ProcessMouseMovement(xoffset, yoffset);
			}
		}
		else
			m_firstMouse = true;
	}

protected:
	// camera
	Camera m_camera;
	float m_lastX, m_lastY;
	bool m_mouseDown = true;
	bool m_firstMouse = true;
	// timing
	float m_deltaTime;	// time between current frame and last frame
	float m_lastFrame;

	bool m_active;
};