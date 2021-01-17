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
#include "../camera/CameraSystem.h"
#include "../graphics/GeometryTools.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <boost/serialization/access.hpp>

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
	void setPosition(const CameraPos& p)
	{
		m_position = p.position;
		m_orientation = Geometry::ypr(p.orientation);
		updateCameraVectors();
	}
	CameraPos position()
	{
		return CameraPos{m_position,m_orientation};
	}

	glm::vec3 m_position;
	Geometry::YawPitchRoll m_orientation;

	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldup;

	float m_movementSpeed;
	float m_mouseSensitivity;
	float m_zoom;

	float m_nearZ;
	float m_farZ;
	float m_aspectRatio;

	Camera(glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f }, Geometry::YawPitchRoll ypr = glm::vec3{ -90.f,0.f,0.f }, glm::vec3 up = glm::vec3{ 0.0f, 1.0f, 0.0f })
		:
		m_movementSpeed(SPEED),
		m_mouseSensitivity(SENSITIVITY),
		m_zoom(ZOOM),
		m_aspectRatio{ 1920.f / 1080.f },
		m_nearZ{ 0.1f },
		m_farZ{ 1000.f },

		m_position{ position },
		m_orientation{ ypr },
		m_worldup{ 0.f,1.f,0.f },

		m_front{ 0.f,0.f,-1.f },
		m_right{ 1.f,0.f,0.f },
		m_up{ up }
	{
	}
	__inline glm::vec3 front() const { return m_front; };
	__inline glm::vec3 up() const { return m_up; };
	__inline glm::vec3 right() const { return m_right; };

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
		m_orientation = Geometry::YawPitchRoll{ -90.f,0.f,0.f };
		updateCameraVectors();
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainm_pitch = true)
	{
		xoffset *= m_mouseSensitivity;
		yoffset *= m_mouseSensitivity;

		m_orientation[0] += xoffset;
		m_orientation[1] += yoffset;

		//if (constrainm_pitch)
		//{
		if (m_orientation[1] > 89.0f)
			m_orientation[1] = 89.0f;
		if (m_orientation[1] < -89.0f)
			m_orientation[1] = -89.0f;
		//}

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

	void updateCameraVectors()
	{
		// via https://math.stackexchange.com/questions/1637464/find-unit-vector-given-roll-pitch-and-yaw
		//front.x = -(sin(glm::radians(m_orientation[2])) * cos(glm::radians(m_orientation[0]))) - (cos(glm::radians(m_orientation[2])) * sin(glm::radians(m_orientation[1])) * sin(glm::radians(m_orientation[0])));
		//front.y = (sin(glm::radians(m_orientation[2])) * sin(glm::radians(m_orientation[0]))) - (cos(glm::radians(m_orientation[2])) * sin(glm::radians(m_orientation[1])) * cos(glm::radians(m_orientation[0])));
		//front.z = cos(glm::radians(m_orientation[2])) * cos(glm::radians(m_orientation[1]));
		glm::vec3 front;//
		front.x = cos(glm::radians(m_orientation[0])) * cos(glm::radians(m_orientation[1]));
		front.y = sin(glm::radians(m_orientation[1]));
		front.z = sin(glm::radians(m_orientation[0])) * cos(glm::radians(m_orientation[1]));
		m_front = glm::normalize(front);
		m_right = glm::normalize(glm::cross(m_front, m_worldup));
		m_up = glm::normalize(glm::cross(m_right, m_front));
	}




	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& m_movementSpeed;
		ar& m_mouseSensitivity;
		ar& m_zoom;
		ar& m_aspectRatio;
		ar& m_nearZ;
		ar& m_farZ;

		ar& m_position;
		ar& m_orientation;
		ar& m_worldup;

		ar& m_front;
		ar& m_right;
		ar& m_up;
	}

};