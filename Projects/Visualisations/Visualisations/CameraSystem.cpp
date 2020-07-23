#include "CameraSystem.h"
#include "GeometryTools.h"
#include <GLM/gtc/quaternion.hpp>
#include <imgui/imgui.h>
#include <string>

CameraPos interpolation(const CameraPos& p1, const CameraPos& p2, float f_t)
{
	CameraPos result
	{
		(f_t * p1.position) + ((1.f - f_t) * p2.position),
		glm::slerp(p1.orientation, p2.orientation, f_t) // note: check that result is always normalised.
	};
	return result;
}

float InterpolatedEvent::value(float t)
{
	return
		(t > t_begin) ?
		((t < t_end) ?
			interp.value((t - t_begin) / (t_end - t_begin))
			: 1.f)
		: 0.f;
}

CameraPos CameraSystem::cameraPos(float t) const
{
	CameraPos result{ m_begin };
	result.orientation *= rotationTransformation(t);
	result.position += positionTransformation(t);
	return result;
}

glm::vec3 CameraSystem::positionTransformation(float t) const // easier because position changes commute.
{
	glm::vec3 result{ 0.f };
	for (PositionEvent p : m_positionEvents)
		result += (p.first.value(t) * p.second); // n.b: might want to change value() or provide vec3 return somehow
	return result;
}

glm::quat CameraSystem::rotationTransformation(float t) const // DRAFT FUNCTION.
{
	glm::quat result{ 1.f,glm::vec3{0.f} };
	assert(glm::normalize(result) == result); // Remove this on first run, just checking precision doesn't mess with assertion.
	for (RotationEvent r : m_rotationEvents)
	{
		if (r.first.finished(t))
			for (const glm::quat & q : r.second)	// TODO: SWAP DIRECTIONS
				result *= q;
		else if (r.first.started(t))
		{
			double partialProgress{ r.first.value(t) };
			glm::quat partialQuaternion{ 1.f,glm::vec3{0.f} };
			for (glm::quat q : r.second)
				partialQuaternion *= q;
			result = glm::slerp(result, partialQuaternion, (float)partialProgress);
		}
	}
	double debugTest{ glm::length(result) };
	//assert(glm::normalize(result) == result);
	return result;
}

void InterpolatedEvent::drawUI(const std::string& name)
{
	ImGui::SliderFloat(std::string{ "t_0 " + name }.c_str(), &t_begin, 0.f, 100.f);
	ImGui::SliderFloat(std::string{ "t_f " + name }.c_str(), &t_end, t_begin, t_begin + 10.f); // TODO: EVENTS LONGER THAN 10s (use rescaling ui)
	interp.drawUI(name);
}

void CameraSystem::drawUI()
{
	ImGui::Begin("Camera System");

	//std::vector<bool> posDeletion{}, rotDeletion{};

	ImGui::Text("Initial Camera Pos");
	ImGui::SliderFloat3("Start Pos", &m_begin.position[0], -10.f, 10.f);
	glm::quat _orient{ m_begin.orientation };
	ImGui::SliderFloat4("Start Orientation: Quat", &_orient[0], -10.f, 10.f);
	Geometry::YawPitchRoll _ypr{ Geometry::ypr(m_begin.orientation) };
	ImGui::SliderFloat3("Start Orientation: Yaw/Pitch/Roll", &_ypr[0], 0.f, 360.f);
	if (_orient != m_begin.orientation)
		m_begin.orientation = glm::normalize(_orient);
	else if (_ypr != Geometry::ypr(m_begin.orientation))
		m_begin.orientation = Geometry::quat(_ypr);

	ImGui::Text("Position Events");
	ImGui::SameLine();
	bool addPosition{ ImGui::Button("add p") };
	ImGui::SameLine();
	bool remPosition{ ImGui::Button("rem p") };
	ImGui::SameLine();
	ImGui::Text(std::string{ "pTotal: " + std::to_string(m_positionEvents.size()) }.c_str());
	ImGui::InputInt("editing p-event #:", &ui_currentPositionEvent, 1);

	//for (PositionEvent& p : m_positionEvents)
	//{
	//	p.first.drawUI();
	//	ImGui::SliderFloat3("Movement", &p.second[0], 0.f, 5.f); // TODO: Positions larger than 5 (see above)
	//	posDeletion.push_back(ImGui::Button("-"));
	//}
	if (ui_currentPositionEvent < m_positionEvents.size()
		&& ui_currentPositionEvent >= 0)
	{
		PositionEvent& p{ m_positionEvents[ui_currentPositionEvent] };
		p.first.drawUI(std::string{ "p" + std::to_string(ui_currentPositionEvent) });
		ImGui::SliderFloat3(std::string{ "movement" + std::to_string(ui_currentPositionEvent) }.c_str(),
			&p.second[0],
			-5.f * (float)ui_movementScale, 5.f * (float)ui_movementScale);
		ImGui::SameLine();
		if (ImGui::Button("mscale+"))
			ui_movementScale++;
		ImGui::SameLine();
		if (ImGui::Button("mscale-"))
			(ui_movementScale > 1) ? ui_movementScale-- : ui_movementScale;
	}
	else
		ui_currentPositionEvent = 0;


	ImGui::Text("Rotation Events");
	ImGui::SameLine();
	bool addRotation{ ImGui::Button("add r") };
	ImGui::SameLine();
	bool remRotation{ ImGui::Button("rem r") };
	ImGui::SameLine();
	ImGui::Text(std::string{ "rTotal: " + std::to_string(m_rotationEvents.size()) }.c_str());
	ImGui::InputInt("editing r-event #:", &ui_currentRotationEvent, 1);


	if (ui_currentRotationEvent < m_rotationEvents.size()
		&& ui_currentRotationEvent >= 0)
	{
		RotationEvent& r{ m_rotationEvents[ui_currentRotationEvent] };
		r.first.drawUI(std::string{ "r" + std::to_string(ui_currentRotationEvent) });
		for (glm::quat& q : r.second)
		{
			glm::quat& qslide{ q };
			if (ui_yprMode)
			{
				Geometry::YawPitchRoll yprSlide{ Geometry::ypr(qslide) };
				ImGui::SliderFloat3("Yaw/Pitch/Roll", &yprSlide[0], -2.f * 3.14159f, 2.f * 3.14159f);
				if (yprSlide != Geometry::ypr(q))
					q = glm::normalize(Geometry::quat(yprSlide));
			}
			else
			{
				ImGui::SliderFloat4("quaternion", &qslide[0], -1.f, 1.f);
				if (qslide != q)
					q = glm::normalize(qslide);
			}
			ImGui::SameLine();
			if (ImGui::Button("switch input"))
				ui_yprMode = !ui_yprMode;
		}
	}
	else
		ui_currentRotationEvent = 0;


	//for (RotationEvent& r : m_rotationEvents)
	//{
	//	r.first.drawUI();
	//	for (glm::quat& q : r.second)
	//	{
	//		glm::quat& qslide{ q };
	//		ImGui::SliderFloat4("quaternion", &qslide[0], -1.f, 1.f);
	//		if (qslide != q)
	//			q = glm::normalize(qslide);
	//	}
	//	rotDeletion.push_back(ImGui::Button("-"));
	//}
	ImGui::End();


	if (addPosition)
		m_positionEvents.push_back(PositionEvent{});
	if (addRotation)
	{
		m_rotationEvents.push_back(RotationEvent{});
		m_rotationEvents.back().second.resize(1);
	}
	if (remPosition)
		if (ui_currentPositionEvent >= 0 && ui_currentPositionEvent < m_positionEvents.size())
			m_positionEvents.erase(m_positionEvents.begin() + ui_currentPositionEvent);
	if (remRotation)
		if (ui_currentRotationEvent >= 0 && ui_currentRotationEvent < m_rotationEvents.size())
			m_rotationEvents.erase(m_rotationEvents.begin() + ui_currentRotationEvent);


	//// Add/Remove positions and rotations.
	//for (size_t j = 0; j < posDeletion.size(); ++j)
	//	if (posDeletion[j])
	//		m_positionEvents.erase(m_positionEvents.begin() + j);
	//for (size_t j = 0; j < rotDeletion.size(); ++j)
	//	if (rotDeletion[j])
	//		m_rotationEvents.erase(m_rotationEvents.begin() + j);

}
