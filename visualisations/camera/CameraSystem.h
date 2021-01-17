#pragma once
#include "../maths/Interpolation.h"
#include "../general/Serialisation.h"

#include <GLM/glm.hpp>
#include <GLM/gtc/quaternion.hpp>

#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

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

class CameraPos
{
public:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& position;
		ar& orientation;
	}

	CameraPos(glm::vec3 pos = glm::vec3{ 0.0f }, glm::quat orient = glm::quat{}) : position{ pos }, orientation{ orient } {}
	glm::vec3 position; // n.b should this be a change in position or a world space value?
	glm::quat orientation; // always normalised

	bool loadFileTree(const boost::property_tree::ptree& t);
	bool saveFileTree(boost::property_tree::ptree& t) const;

	void clear() {
		position = glm::vec3{};
		orientation = glm::quat{};
	}
};

// NOTE: Unused, pending deletion.
CameraPos interpolation(const CameraPos& p1, const CameraPos& p2, float f_t);

struct InterpolatedEvent
{
	float value(float t) const; // expects t in time units (seconds), values outside bounds are handled.
	bool finished(float t) const { return t >= t_end; }
	bool started(float t) const { return t >= t_begin; }
	Interpolation interp;
	float t_begin, t_end;
	void drawUI(const std::string& name);

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& interp;
		ar& t_begin;
		ar& t_end;
	}

};
struct PositionEvent
{
	InterpolatedEvent first;
	glm::vec3 second;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& first;
		ar& second;
	}

};
struct RotationEvent
{
	InterpolatedEvent first;
	glm::quat second; // removed vect: YAGNI

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& first;
		ar& second;
	}

};

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

	void clear();
	bool loadFileTree(const boost::property_tree::ptree& t);
	bool saveFileTree(boost::property_tree::ptree& t) const;

	CameraPos cameraPos(float t) const;
	glm::vec3 positionTransformation(float t) const;
	glm::quat rotationTransformation(float t) const; // order of simultaneous rotations is made by start time.

	void drawUI();

	CameraPos startPos() const { return m_begin; }
	void setStartPos(const CameraPos& cpos) { m_begin = cpos;}


	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& m_begin;
		ar& m_positionEvents; // TODO
		ar& m_rotationEvents;

		// not necessary
		ar& ui_currentPositionEvent;
		ar& ui_currentRotationEvent;
		ar& ui_yprMode;
		ar& ui_movementScale;
	}

private:
	CameraPos m_begin;
	std::vector<PositionEvent> m_positionEvents;
	std::vector<RotationEvent> m_rotationEvents;

	int ui_currentPositionEvent;
	int ui_currentRotationEvent;
	bool ui_yprMode; // use quaternion or yaw/pitch/roll in ui

	int ui_movementScale;
};
