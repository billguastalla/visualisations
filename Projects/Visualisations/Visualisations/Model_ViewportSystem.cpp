#include "Model_ViewportSystem.h"

Model_ViewportSystem::Model_ViewportSystem(GLFWwindow* win)
	:
	m_window{ win },

	m_camera{ glm::vec3{0.0f, 0.0f, 3.0f} },
	m_mouseDown{ false },
	m_firstMouse{ true },
	m_lastX{ 0.0f },
	m_lastY{ 0.0f },
	m_freeCamera{true}
{
}

void Model_ViewportSystem::mouseMovement(float xPos, float yPos, bool mouseDown)
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

void Model_ViewportSystem::keyMovement(Camera_Movement cm)
{
	m_camera.ProcessKeyboard(cm, 0.01f);
}

void Model_ViewportSystem::processCamera(double time)
{	// TODO: Camera is missing roll. This means it needs free/locked mode to function as a state for quaternion transforms

	CameraPos p{ m_cameraSystem.cameraPos(time) };

	glm::vec3 orient{ glm::eulerAngles(p.orientation) };
	m_camera.m_pitch = orient.x;
	m_camera.m_yaw = orient.y;

	m_camera.m_position = p.position;
	m_camera.m_up = p.orientation * m_camera.m_up;
	m_camera.m_right = p.orientation * m_camera.m_right;
	m_camera.m_front = p.orientation * m_camera.m_front;
}
