//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  This file is an original or modified version of code provided by Joey De Vries. //
//                                                                                  //
//  It is licensed under the CC BY-NC 4.0 license. You can find                     //
//  a copy of this license at the top of the project tree.                          //
//                                                                                  //
//  LearnOpenGL:  https://learnopengl.com/                                          //
//  Github:       https://github.com/JoeyDeVries/                                   //
//  Website:      https://joeydevries.com/                                          //
//  Twitter:      https://twitter.com/JoeyDeVriez                                   //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "CameraSystem.h"
#include "GeometryTools.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <boost/property_tree/ptree_fwd.hpp>

enum class Camera_Movement {
	NONE = 0x00,
	FORWARD = 0x01,
	BACKWARD = 0x02,
	LEFT = 0x04,
	RIGHT = 0x08,
	UP = 0x10,
	INCREASE_MOVEMENT_SPEED = 0x20,
	DECREASE_MOVEMENT_SPEED = 0x40,
	RESET_POSITION = 0x80,
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float ROLL = 0.0f;

const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
	bool loadFileTree(const boost::property_tree::ptree& t);
	bool saveFileTree(boost::property_tree::ptree& t) const;


	// TODO: hide these variables to formalise how they should be used.
	// TODO: Swap ypr for quaternions or concat into vec3.

	void setPosition(CameraPos p)
	{
		m_position = p.position;
		m_orientation = p.orientation;
	}

	glm::vec3 m_position;
	//glm::vec3 m_front;
	//glm::vec3 m_up;
	//glm::vec3 m_right;
	glm::vec3 m_worldup;


	glm::quat m_orientation;
	//float m_yaw;
	//float m_pitch;
	//float m_roll;

	float m_movementSpeed;
	float m_mouseSensitivity;
	float m_zoom;

	float m_nearZ;
	float m_farZ;
	float m_aspectRatio;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		:
		m_movementSpeed(SPEED),
		m_mouseSensitivity(SENSITIVITY),
		m_zoom(ZOOM),
		m_aspectRatio{ 1920.f / 1080.f },
		m_nearZ{ 0.1f },
		m_farZ{ 1000.f }, 
		m_orientation{glm::normalize(glm::quat{0.f,0.f,0.f,1.f})}
	{
		m_position = position;
		m_worldup = up;
	}

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		:
		m_movementSpeed{ SPEED },
		m_mouseSensitivity{ SENSITIVITY },
		m_zoom{ ZOOM },
		m_aspectRatio{ 1920.f / 1080.f },
		m_nearZ{ 0.1f },
		m_farZ{ 100.f },
		m_position{ glm::vec3(posX, posY, posZ) },
		m_worldup{ glm::vec3(upX, upY, upZ) }
	{
		updateCameraVectors();
	}

	__inline glm::vec3 front() const { return  glm::conjugate(m_orientation) * glm::vec3{0.f,0.f,-1.f} *m_orientation ; };
	__inline glm::vec3 up() const { return  glm::conjugate(m_orientation) * glm::vec3{0.f,1.f,0.f} *m_orientation ; };
	__inline glm::vec3 right() const { return  glm::conjugate(m_orientation) * glm::vec3{1.f,0.f,0.f} *m_orientation ; };

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix() const
	{

		return glm::lookAt(m_position, m_position + front(), up());
	}
	glm::mat4 projectionMatrix() const
	{
		return glm::perspective(glm::radians(m_zoom), m_aspectRatio, m_nearZ, m_farZ);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = m_movementSpeed * deltaTime;
		if ((int)direction & (int)Camera_Movement::FORWARD)
			m_position += front() * velocity;
		if ((int)direction & (int)Camera_Movement::BACKWARD)
			m_position -= front() * velocity;
		if ((int)direction & (int)Camera_Movement::LEFT)
			m_position -= right() * velocity;
		if ((int)direction & (int)Camera_Movement::RIGHT)
			m_position += right() * velocity;
		if ((int)direction & (int)Camera_Movement::UP)
			m_position += glm::vec3{ 0.0,1.0,0.0 } *velocity;
		if ((int)direction & (int)Camera_Movement::DECREASE_MOVEMENT_SPEED)
			m_movementSpeed *= 0.95f;
		if ((int)direction & (int)Camera_Movement::INCREASE_MOVEMENT_SPEED)
			m_movementSpeed *= 1.05f;
		if ((int)direction & (int)Camera_Movement::RESET_POSITION)
			resetPosition();
	}

	void resetPosition()
	{
		m_position = glm::vec3{};
		m_orientation = glm::quat{};
		//m_up = glm::vec3(0.0f, 1.0f, 0.0f);

		//m_yaw = YAW;
		//m_pitch = PITCH;
		//m_roll = ROLL;

		updateCameraVectors();
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainm_pitch = true)
	{
		xoffset *= m_mouseSensitivity;
		yoffset *= m_mouseSensitivity;

		Geometry::YawPitchRoll _ypr{ Geometry::ypr(m_orientation) };
		_ypr[0] += xoffset;
		_ypr[1] += yoffset;

		m_orientation = glm::rotate(m_orientation, -xoffset / 10.f, up());
		m_orientation = glm::rotate(m_orientation, -yoffset / 10.f, right());
		m_orientation = glm::normalize(m_orientation);

		//if (constrainm_pitch)
		//{
		//	if (_ypr[1] > 89.0f)
		//		_ypr[1] = 89.0f;
		//	if (_ypr[1] < -89.0f)
		//		_ypr[1] = -89.0f;
		//}
		//m_orientation = Geometry::quat(_ypr);

		//updateCameraVectors();
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
		//glm::vec3 front;
		//front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		//front.y = sin(glm::radians(m_pitch));
		//front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		//m_front = glm::normalize(front);
		//// Also re-calculate the m_right and m_up vector
		//m_right = glm::normalize(glm::cross(m_front, m_worldup));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		//m_up = glm::normalize(glm::cross(m_right, m_front));
	}
};