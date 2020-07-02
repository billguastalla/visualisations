#include "Model_ViewportSystem.h"

Model_ViewportSystem::Model_ViewportSystem(GLFWwindow* win)
	:
	m_window{win},

	m_camera{ glm::vec3{0.0f, 0.0f, 3.0f} },
	m_deltaTime{},
	m_mouseDown{ false },
	m_lastFrame{ 0.0f },
	m_lastX{ 0.0f },
	m_lastY{ 0.0f },
	m_cameraMode{ CameraMode::RotateViewer }
{
}

void Model_ViewportSystem::mouseMovement(float xPos, float yPos, bool mouseDown)
{
	switch (m_cameraMode)
	{
	case CameraMode::RotateViewer:
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
	break;
	case CameraMode::RotateScene:
		break;
	default:
		break;
	}
}

void Model_ViewportSystem::keyMovement(Camera_Movement cm)
{
	m_camera.ProcessKeyboard(cm, 0.01f);
}

void Model_ViewportSystem::processCamera(double time)
{
	CameraPos p{ m_cameraSystem.cameraPos(time) };
	m_camera.m_position = p.position;
	m_camera.m_up = p.orientation * m_camera.m_up;
	m_camera.m_right = p.orientation * m_camera.m_right;
	m_camera.m_front = p.orientation * m_camera.m_front;
}
