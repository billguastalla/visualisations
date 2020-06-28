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
}