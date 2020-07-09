#pragma once
#include "Camera.h"
#include "CameraSystem.h"
#include <boost/property_tree/ptree_fwd.hpp>
struct GLFWwindow;

// Note: Cameras need to be told about aspect ratio changes now, since they handle perspective projection.
class Model_ViewportSystem
{
public:
	Model_ViewportSystem(GLFWwindow* win);

	const Camera& camera() { return m_camera; }
	CameraSystem& cameraSystem() { return m_cameraSystem; }

	void mouseMovement(float xPos, float yPos, bool mouseDown);
	void keyMovement(Camera_Movement cm);

	// iteration function (TODO: should this be done by another class?)
	void processCamera(double time);

	bool freeCamera() { return m_freeCamera; }
	void setFreeCamera(bool f) { m_freeCamera = f; }


	bool loadFileTree(const boost::property_tree::ptree & t);
	bool saveFileTree(boost::property_tree::ptree& t) const;
private:
	GLFWwindow* m_window;

	bool m_freeCamera;
	CameraSystem m_cameraSystem;
	Camera m_camera; // TODO: Convert to set of cameras

	float m_lastX, m_lastY;		
	bool m_mouseDown;	
	bool m_firstMouse;	
};