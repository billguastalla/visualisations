#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	NONE = 0x00,
	FORWARD = 0x01,
	BACKWARD = 0x02,
	LEFT = 0x04,
	RIGHT = 0x08,
	INCREASE_MOVEMENT_SPEED = 0x10,
	DECREASE_MOVEMENT_SPEED = 0x20,
	RESET_POSITION = 0x40,
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldup;
	// Euler Angles
	float m_yaw;
	float m_pitch;
	// Camera options
	float m_movementSpeed;
	float m_mouseSensitivity;
	float m_zoom;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		: 
		m_front(glm::vec3(0.0f, 0.0f, -1.0f)), 
		m_movementSpeed(SPEED), 
		m_mouseSensitivity(SENSITIVITY), 
		m_zoom(ZOOM)
	{
		m_position = position;
		m_worldup = up;
		m_yaw = yaw;
		m_pitch = pitch;
		updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		: 
		m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
		m_movementSpeed(SPEED), 
		m_mouseSensitivity(SENSITIVITY), 
		m_zoom(ZOOM)
	{
		m_position = glm::vec3(posX, posY, posZ);
		m_worldup = glm::vec3(upX, upY, upZ);
		m_yaw = yaw;
		m_pitch = pitch;
		updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = m_movementSpeed * deltaTime;
		if (direction & FORWARD)
			m_position += m_front * velocity;
		if (direction & BACKWARD)
			m_position -= m_front * velocity;
		if (direction & LEFT)
			m_position -= m_right * velocity;
		if (direction & RIGHT)
			m_position += m_right * velocity;
		if (direction & Camera_Movement::DECREASE_MOVEMENT_SPEED)
			m_movementSpeed *= 0.95;
		if (direction & Camera_Movement::INCREASE_MOVEMENT_SPEED)
			m_movementSpeed *= 1.05;
		if (direction & Camera_Movement::RESET_POSITION)
			resetPosition();
	}

	void resetPosition()
	{
		m_position = glm::vec3(0.0f, 0.0f, 0.0f);
		m_up = glm::vec3(0.0f, 1.0f, 0.0f);
		updateCameraVectors();
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainm_pitch = true)
	{
		xoffset *= m_mouseSensitivity;
		yoffset *= m_mouseSensitivity;

		m_yaw += xoffset;
		m_pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainm_pitch)
		{
			if (m_pitch > 89.0f)
				m_pitch = 89.0f;
			if (m_pitch < -89.0f)
				m_pitch = -89.0f;
		}

		// m_update m_front, m_right and m_up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (m_zoom >= 1.0f && m_zoom <= 45.0f)
			m_zoom -= yoffset;
		if (m_zoom <= 1.0f)
			m_zoom = 1.0f;
		if (m_zoom >= 45.0f)
			m_zoom = 45.0f;
	}

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new m_front vector
		glm::vec3 front;
		front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.y = sin(glm::radians(m_pitch));
		front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_front = glm::normalize(front);
		// Also re-calculate the m_right and m_up vector
		m_right = glm::normalize(glm::cross(m_front, m_worldup));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		m_up = glm::normalize(glm::cross(m_right, m_front));
	}
};