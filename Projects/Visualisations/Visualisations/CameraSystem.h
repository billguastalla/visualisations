#pragma once
#include <GLM/glm.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <vector>
/*
	Design limitations:
		-> Implementing rotations as an single interpolated quaternion
			limits the ability to control relative rates of rotation components
			e.g. yaw vs. roll.
	Considerations:
		-> Option to pop finished transformations and save an internal position/orientation in the camera system.
*/

struct CameraPos
{
	CameraPos(glm::vec3 pos = glm::vec3{ 0.0f }, glm::quat orient = glm::quat{}) : position{ pos }, orientation{ orient } {}
	glm::vec3 position; // n.b should this be a change in position or a world space value?
	glm::quat orientation; // always normalised
};

// NOTE: Unused, pending deletion.
CameraPos interpolation(const CameraPos& p1, const CameraPos& p2, float f_t);

struct Interpolation // TODO: Allow different coefficient sizes in UI
{
	enum class FunctionType { Polynomial, Exponential, Sinusoidal };
	Interpolation() : m_functionType{ FunctionType::Polynomial }, m_coefficients{ {0.f,1.f,0.f,0.f} } {}
	// TODO: Fix duplicated code, do more rigorous analysis..
	float value(float x); // expects x between [0,1], undefined behaviour outside.
	FunctionType m_functionType;
	std::vector<float> m_coefficients;
	void drawUI();
};
struct InterpolatedEvent
{
	float value(float t); // expects t in time units (seconds), values outside bounds are handled.
	bool finished(float t) { return t >= t_end; }
	bool started(float t) { return t >= t_begin; }
	Interpolation interp;
	float t_begin, t_end;
	void drawUI();
};
typedef std::pair<InterpolatedEvent, glm::vec3> PositionEvent;
typedef std::pair<InterpolatedEvent, std::vector<glm::quat>> RotationEvent; // order of rotation multiplications is given in vector.
// Could add orbit event..

class CameraSystem
{
public:
	CameraSystem() : m_positionEvents{}, m_rotationEvents{} {}
	CameraPos cameraPos(const CameraPos & start, float t) const;
	glm::vec3 positionTransformation(float t) const;
	glm::quat rotationTransformation(float t) const; // order of simultaneous rotations is made by start time.

	void drawUI();
private:

	std::vector<PositionEvent> m_positionEvents;
	std::vector<RotationEvent> m_rotationEvents;
};
