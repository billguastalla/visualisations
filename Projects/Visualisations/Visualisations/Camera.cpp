#include "Camera.h"
#include "Serialisation.h"
#include <boost/property_tree/ptree.hpp>
bool Camera::loadFileTree(const boost::property_tree::ptree& t)
{
	return false;
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
