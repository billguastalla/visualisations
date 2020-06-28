#include "GeometryTools.h"
#include "Constants.h"

std::pair<glm::vec3, glm::vec3> Geometry::normalBasis(glm::vec3 vector)
{
	glm::vec3 rotationAxis{ 1.,0.,0. };
	double angle{ PI / 4.0 };
	glm::quat rotation{ axisAngleToQuat(angle,rotationAxis) };
	rotation = glm::normalize(rotation);
	glm::vec3 normalBasis1{ glm::normalize(rotation * vector * glm::conjugate(rotation)) };
	glm::vec3 normalBasis2{ glm::normalize(glm::cross(glm::normalize(vector),normalBasis1)) }; // outer normalisation should be redundant.
	return std::pair<glm::vec3, glm::vec3>{normalBasis1,normalBasis2};
}

glm::quat Geometry::axisAngleToQuat(double angle, glm::vec3 axis)
{
	return glm::quat{ cosf(angle / 2.0),axis.x * sinf(angle / 2.0),axis.y * sinf(angle / 2.0),axis.z * sinf(angle / 2.0) };
};