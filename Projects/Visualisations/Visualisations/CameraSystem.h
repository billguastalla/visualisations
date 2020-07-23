#pragma once
#include "Interpolation.h"
#include <GLM/glm.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <vector>
#include <string>
/*
	Design limitations:
		-> Implementing rotations as an single interpolated quaternion
			limits the ability to control relative rates of rotation components
			e.g. yaw vs. roll.
				-> Quaternions also have an issue with precision at certain angles!
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

struct InterpolatedEvent
{
	float value(float t); // expects t in time units (seconds), values outside bounds are handled.
	bool finished(float t) { return t >= t_end; }
	bool started(float t) { return t >= t_begin; }
	Interpolation interp;
	float t_begin, t_end;
	void drawUI(const std::string& name);
};
typedef std::pair<InterpolatedEvent, glm::vec3> PositionEvent;
typedef std::pair<InterpolatedEvent, std::vector<glm::quat>> RotationEvent; // order of rotation multiplications is given in vector.
// Could add orbit event..

class CameraSystem
{
public:
	CameraSystem() :
		m_positionEvents{},
		m_rotationEvents{},

		ui_currentPositionEvent{ 0 },
		ui_currentRotationEvent{ 0 },
		ui_movementScale{ 1 },
		ui_yprMode{false}
	{}
	CameraPos cameraPos(float t) const;
	glm::vec3 positionTransformation(float t) const;
	glm::quat rotationTransformation(float t) const; // order of simultaneous rotations is made by start time.

	void drawUI();
private:
	CameraPos m_begin;
	std::vector<PositionEvent> m_positionEvents;
	std::vector<RotationEvent> m_rotationEvents;

	int ui_currentPositionEvent;
	int ui_currentRotationEvent;
	bool ui_yprMode; // use quaternion or yaw/pitch/roll in ui

	int ui_movementScale;
};
