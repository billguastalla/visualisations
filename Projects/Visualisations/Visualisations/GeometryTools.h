#pragma once
#include <GLM/glm.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <vector>

namespace Geometry
{
	// Build a normalised basis for a plane orthogonal to the passed vector
	std::pair<glm::vec3, glm::vec3> normalBasis(glm::vec3 vector);

	// Get a quaternion from an axis and an angle of rotation
	glm::quat axisAngleToQuat(double angle, glm::vec3 axis);

	using YawPitchRoll = glm::vec3;
	YawPitchRoll ypr(const glm::quat& q);
	glm::quat quat(const YawPitchRoll & _ypr);

	//glm::mat3 rotMatrix(const YawPitchRoll& _ypr);

}