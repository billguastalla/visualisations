#pragma once
#include "Camera.h"
#include "Timekeeping.h"
#include "CameraSystem.h"
#include <boost/property_tree/ptree_fwd.hpp>
struct GLFWwindow;

// Note: Cameras need to be told about aspect ratio changes now, since they handle perspective projection.
class Model_ViewportSystem
{
public:
	Model_ViewportSystem(GLFWwindow* win);

	Camera& camera() { return m_camera; } // nb non-const for window
	CameraSystem& cameraSystem() { return m_cameraSystem; }

	void mouseMovement(float xPos, float yPos, bool mouseDown);
	void keyMovement(Camera_Movement cm);

	void processCamera(Timestep ts);

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

public:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& m_freeCamera;
		ar& m_cameraSystem;
		ar& m_camera;
	}

};