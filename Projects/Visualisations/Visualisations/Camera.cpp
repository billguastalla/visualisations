#include "Camera.h"
#include <boost/property_tree/ptree.hpp>
bool Camera::loadFileTree(const boost::property_tree::ptree& t)
{
	return false;
}

bool Camera::saveFileTree(boost::property_tree::ptree& t) const
{
	auto glmToString = [](const glm::vec3& v)
	{
		std::string res{};
		res += std::to_string(v.x) + ",";
		res += std::to_string(v.y) + ",";
		res += std::to_string(v.z);
		return res;
	};
	auto glmFromString = [](const std::string& s) // DRAFT
	{
		glm::vec3 res{ 0.f };
		size_t sep1{ s.find(",") };
		size_t sep2{ s.find(",",sep1) };
		std::string v1{ s.substr(0,sep1) };
		std::string v2{ s.substr(sep1,sep2 - sep1) };
		std::string v3{ s.substr(sep2,s.size() - sep2) };
		res.x = std::stof(v1);
		res.y = std::stof(v2);
		res.z = std::stof(v3);
	};

	t.put("camera.mouseSensitivity", m_mouseSensitivity);
	t.put("camera.movementSpeed", m_movementSpeed);
	t.put("camera.aspectRatio", m_aspectRatio);
	t.put("camera.nearZ", m_nearZ);
	t.put("camera.farZ", m_farZ);
	t.put("camera.position", glmToString(m_position));
	t.put("camera.front", glmToString(m_front));
	t.put("camera.right", glmToString(m_right));
	t.put("camera.up", glmToString(m_up));
	t.put("camera.worldup", glmToString(m_worldup));
	t.put("camera.pitch", m_pitch);
	t.put("camera.yaw", m_yaw);
	t.put("camera.zoom", m_zoom);
	return true;
}
