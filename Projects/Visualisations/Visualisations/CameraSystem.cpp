#include "CameraSystem.h"
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

float Interpolation::value(float x) // input is \in [0,1]
{
	float result{ 0.f };
	float normalisation{ 0.f };
	switch (m_functionType)
	{
	case FunctionType::Polynomial:
	{
		int degree{ 0 };
		for (float c : m_coefficients)
		{
			result += c * pow(x, degree);
			normalisation += c; // pow(1,k) = 1 \forall k
			++degree;
		}
	}
	break;
	case FunctionType::Exponential:
	{
		int degree{ 0 };
		for (float c : m_coefficients)
		{
			result += std::exp(c * pow(x, degree));
			normalisation += std::exp(c);
			++degree;
		}
	}
	break;
	case FunctionType::Sinusoidal:
	{
		int degree{ 0 };
		for (float c : m_coefficients)
		{
			result += std::sin(c * pow(x, degree));
			normalisation += std::sin(c);
			++degree;
		}
	}
	break;
	}
	return (normalisation != 0.f) ? (result / normalisation) : 0.f; // avoid division by zero
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
	CameraPos result{};
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
			for (glm::quat q : r.second)	// TODO: SWAP DIRECTIONS
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
	assert(glm::normalize(result) == result);
	return result;
}

void Interpolation::drawUI()
{

	for (size_t i{ 0 }; i < m_coefficients.size(); ++i)
		ImGui::SliderFloat(std::string{ "c" + std::to_string(i) }.c_str(), &m_coefficients[i], 0.f, 1.f);
	// TODO: Safety check of coefficients to prevent division by zero.
}

void InterpolatedEvent::drawUI()
{
	ImGui::SliderFloat("Start Time", &t_begin, 0.f, 100.f);
	ImGui::SliderFloat("End Time", &t_end, t_begin, t_begin + 10.f); // TODO: EVENTS LONGER THAN 10s (use rescaling ui)
	interp.drawUI();
}

void CameraSystem::drawUI()
{
	ImGui::Begin("Camera System");

	std::vector<bool> posDeletion{}, rotDeletion{};

	ImGui::Text("Position Events");
	ImGui::SameLine();
	bool addPostition{ ImGui::Button("+p") };
	for (PositionEvent& p : m_positionEvents)
	{
		p.first.drawUI();
		ImGui::SliderFloat3("Movement", &p.second[0], 0.f, 5.f); // TODO: Positions larger than 5 (see above)
		posDeletion.push_back(ImGui::Button("-"));
	}
	ImGui::Text("Rotation Events");
	ImGui::SameLine();
	bool addRotation{ ImGui::Button("+r") };
	for (RotationEvent& r : m_rotationEvents)
	{
		r.first.drawUI();
		for (glm::quat& q : r.second)
		{
			glm::quat& qslide{ q };
			ImGui::SliderFloat4("quaternion", &qslide[0], -1.f, 1.f);
			if (qslide != q)
				q = glm::normalize(qslide);
		}
		rotDeletion.push_back(ImGui::Button("-"));
	}
	ImGui::End();

	// Add/Remove positions and rotations.
	for (size_t j = 0; j < posDeletion.size(); ++j)
		if (posDeletion[j])
			m_positionEvents.erase(m_positionEvents.begin() + j);
	for (size_t j = 0; j < rotDeletion.size(); ++j)
		if (rotDeletion[j])
			m_rotationEvents.erase(m_rotationEvents.begin() + j);
	if (addPostition)
		m_positionEvents.push_back(PositionEvent{});
	if (addRotation)
	{
		m_rotationEvents.push_back(RotationEvent{});
		m_rotationEvents.back().second.resize(1);
	}
}
