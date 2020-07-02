#pragma once
#include "Camera.h"
#include "CameraSystem.h"

struct GLFWwindow;

enum class CameraMode
{
	RotateViewer,
	RotateScene
};

// Note: Cameras need to be told about aspect ratio changes now, since they handle perspective projection.
class Model_ViewportSystem
{
public:
	Model_ViewportSystem(GLFWwindow* win);

	const Camera& camera() { return m_camera; }
	glm::vec3 cameraPosition() const { return m_camera.m_position; }

	void setCameraMode(const CameraMode& m) { m_cameraMode = m; };
	void mouseMovement(float xPos, float yPos, bool mouseDown);
	void keyMovement(Camera_Movement cm);

	// iteration function
	void processCamera(double time);
private:
	GLFWwindow* m_window;

	CameraSystem m_cameraSystem;

	Camera m_camera;			
	CameraMode m_cameraMode;	
	float m_lastX, m_lastY;		
	bool m_mouseDown = true;	
	bool m_firstMouse = true;	
	float m_deltaTime;
	float m_lastFrame;


};