#include "Visualisation.h"

void Visualisation::mouseMovement(float xPos, float yPos, bool mouseDown)
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

void Visualisation::keyMovement(Camera_Movement cm)
{
	m_camera.ProcessKeyboard(cm, 0.01);
}
