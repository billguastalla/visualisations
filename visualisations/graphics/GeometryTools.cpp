#include "GeometryTools.h"
#include "../general/Constants.h"

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
	return glm::quat{ cosf(angle / 2.f),axis.x * sinf(angle / 2.f),axis.y * sinf(angle / 2.f),axis.z * sinf(angle / 2.f) };
}

Geometry::YawPitchRoll Geometry::ypr(const glm::quat& q)
{
	return Geometry::YawPitchRoll{glm::yaw(q),glm::pitch(q),glm::roll(q)};
}

glm::quat Geometry::quat(const Geometry::YawPitchRoll& _ypr)
{
	// Taken from https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

	// Abbreviations for the various angular functions
	double cy = cos(_ypr[0]* 0.5);
	double sy = sin(_ypr[0] * 0.5);
	double cp = cos(_ypr[1] * 0.5);
	double sp = sin(_ypr[1] * 0.5);
	double cr = cos(_ypr[2] * 0.5);
	double sr = sin(_ypr[2] * 0.5);

	glm::quat q{};
	q.w = cr * cp * cy + sr * sp * sy;
	q.x = sr * cp * cy - cr * sp * sy;
	q.y = cr * sp * cy + sr * cp * sy;
	q.z = cr * cp * sy - sr * sp * cy;
	return q;
}

//glm::mat3 Geometry::rotMatrix(const YawPitchRoll& _ypr)// https://en.wikipedia.org/wiki/Rotation_matrix
//{
//	return glm::mat3 {
//		cos(glm::radians(_ypr[0])) * cos(glm::radians(_ypr[1])),
//		(cos(glm::radians(_ypr[0])) * sin(glm::radians(_ypr[1])) * sin(glm::radians(_ypr[2]))) - (sin(glm::radians(_ypr[0])) * cos(glm::radians(_ypr[2]))),
//		(cos(glm::radians(_ypr[0])) * sin(glm::radians(_ypr[1])) * cos(glm::radians(_ypr[2]))) + (sin(glm::radians(_ypr[0])) * sin(glm::radians(_ypr[2]))),
//
//		sin(glm::radians(_ypr[0])) * cos(glm::radians(_ypr[1])),
//		(sin(glm::radians(_ypr[0])) * sin(glm::radians(_ypr[1])) * sin(glm::radians(_ypr[2]))) + (cos(glm::radians(_ypr[0])) * cos(glm::radians(_ypr[2]))),
//		(sin(glm::radians(_ypr[0])) * sin(glm::radians(_ypr[1])) * cos(glm::radians(_ypr[2]))) - (cos(glm::radians(_ypr[0])) * sin(glm::radians(_ypr[2]))),
//
//		-sin(glm::radians(_ypr[1])),
//		cos(glm::radians(_ypr[1])) * sin(glm::radians(_ypr[2])),
//		cos(glm::radians(_ypr[1])) * cos(glm::radians(_ypr[2]))
//	};
//}
