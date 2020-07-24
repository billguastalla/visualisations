#include "Camera.h"
#include "Serialisation.h"
#include <boost/property_tree/ptree.hpp>
bool Camera::loadFileTree(const boost::property_tree::ptree& t)
{
	m_mouseSensitivity = t.get<double>("camera.mouseSensitivity",m_mouseSensitivity);
	m_movementSpeed = t.get<float>("camera.movementSpeed", m_movementSpeed);
	m_aspectRatio = t.get<float>("camera.aspectRatio", m_aspectRatio);
	m_nearZ = t.get<float>("camera.nearZ", m_nearZ);
	m_farZ = t.get<float>("camera.farZ", m_farZ);
	m_position = Serialisation::vec3glmFromString(t.get<std::string>("camera.position", Serialisation::vec3glmToString(m_position)));
	m_worldup = Serialisation::vec3glmFromString(t.get<std::string>("camera.worldup", Serialisation::vec3glmToString(m_worldup)));
	m_zoom = t.get<float>("camera.zoom", m_zoom);
	return true;
}

bool Camera::saveFileTree(boost::property_tree::ptree& t) const
{
	t.put("camera.mouseSensitivity", m_mouseSensitivity);
	t.put("camera.movementSpeed", m_movementSpeed);
	t.put("camera.aspectRatio", m_aspectRatio);
	t.put("camera.nearZ", m_nearZ);
	t.put("camera.farZ", m_farZ);
	t.put("camera.position", Serialisation::vec3glmToString(m_position));
	//t.put("camera.front", glmToString(m_front));
	//t.put("camera.right", glmToString(m_right));
	//t.put("camera.up", glmToString(m_up));
	t.put("camera.worldup", Serialisation::vec3glmToString(m_worldup));
	//t.put("camera.pitch", m_pitch);
	//t.put("camera.yaw", m_yaw);
	t.put("camera.zoom", m_zoom);
	return true;
}
