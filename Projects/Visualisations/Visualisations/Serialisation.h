#pragma once
#include <GLM/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>

namespace Serialisation
{
	auto vec3glmToString = [](const glm::vec3& v)
	{
		std::string res{};
		res += std::to_string(v.x) + ",";
		res += std::to_string(v.y) + ",";
		res += std::to_string(v.z);
		return res;
	};
	auto vec3glmFromString = [](const std::string& s) // DRAFT
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
		return res;
	};
	auto quatglmToString = [](const glm::quat& v)
	{
		std::string res{};
		res += std::to_string(v[0]) + ",";
		res += std::to_string(v[1]) + ",";
		res += std::to_string(v[2]) + ",";
		res += std::to_string(v[3]);
		return res;
	};
	auto quatglmFromString = [](const std::string& s) // DRAFT
	{
		glm::quat res{ 0.f,0.f,0.f,1.f };
		size_t sep1{ s.find(",") };
		size_t sep2{ s.find(",",sep1) };
		size_t sep3{ s.find(",",sep2) };
		std::string q1{ s.substr(0,sep1) };
		std::string q2{ s.substr(sep1,sep2 - sep1) };
		std::string q3{ s.substr(sep2,s.size() - sep2) };
		std::string q4{ s.substr(sep3,s.size() - sep3) }; // CHECK THIS!
		res[0] = std::stof(q1);
		res[1] = std::stof(q2);
		res[2] = std::stof(q3);
		res[3] = std::stof(q4);
		return res;
	};
	template <class T> // note: move to general functions if reused.
	auto floatingVectToStr = [](const std::vector<T>& t) {
		std::string res{};
		for (const T& a : t)
			res += std::to_string(a) + ",";
		if (res.size() > 0)
			res = res.substr(0u, res.size() - 1); // erase would be faster.
		return res;
	};

	template <class T>
	auto strToFloatingVect = [](const std::string& s) {
		std::vector<T> res;
		size_t p_b{ 0u }, p_f{ s.find(",", 0u) };
		while (p_f != std::string::npos)
		{
			res.push_back(std::stod(s.substr(p_b, p_f - p_b)));
			p_b = p_f;
			p_f = s.find(",", p_b);
		}
		res.push_back(std::stod(s.substr(p_b)));
		return res;
	};


}