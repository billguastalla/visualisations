#pragma once
#include <string>
#include "Camera.h"

class Buffer;
enum class CameraMode
{
	RotateViewer,
	RotateScene
};

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
		m_lastY{ 0.0f },
		m_cameraMode{CameraMode::RotateViewer}
	{};

	/* Set up shaders, initial VAOs, VBOs */
	virtual void activate() = 0;
	/* Unload shaders etc. */
	virtual void deactivate() = 0;

	virtual void processSamples(const Buffer& buf, unsigned samples) = 0;
	virtual void renderFrame() = 0;
	virtual void drawInterface() {};

	virtual std::string titleString() = 0;


	void mouseMovement(float xPos, float yPos, bool mouseDown);
	void keyMovement(Camera_Movement cm);

	void setCameraMode(const CameraMode & m) { m_cameraMode = m; };
	glm::vec3 cameraPosition() const { return m_camera.m_position; }
protected:
	// camera
	Camera m_camera;
	CameraMode m_cameraMode;

	float m_lastX, m_lastY;
	bool m_mouseDown = true;
	bool m_firstMouse = true;
	// timing
	float m_deltaTime;	// time between current frame and last frame
	float m_lastFrame;

	bool m_active;
};