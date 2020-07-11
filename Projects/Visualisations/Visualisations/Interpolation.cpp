#include "Interpolation.h"
#include <imgui/imgui.h>
#include <cmath>
#include <boost/property_tree/ptree.hpp>

constexpr char funcTypeStr[] = "Polynomial\0Exponential\0Sinusoidal\0";

template <class T>
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
		res.push_back(std::stod(s.substr(p_b,p_f - p_b)));
		p_b = p_f;
		p_f = s.find(",", p_b);
	}
	res.push_back(std::stod(s.substr(p_b)));
	return res;
};

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

void Interpolation::drawUI(const std::string& name)
{
	int f{ (int)m_functionType };
	ImGui::Combo("Function", &f, &funcTypeStr[0]);
	for (size_t i{ 0 }; i < m_coefficients.size(); ++i)
		ImGui::SliderFloat(std::string{ "c" + std::to_string(i) + " " + name }.c_str(), &m_coefficients[i], 0.f, 1.f);
	// TODO: Safety check of coefficients to prevent division by zero.
}

bool Interpolation::loadFileTree(const boost::property_tree::ptree& t)
{
	m_coefficients = strToFloatingVect<float>(t.get<std::string>("coefficients"));
	m_functionType = (FunctionType)t.get<int>("functionType");
	return true;
}

bool Interpolation::saveFileTree(boost::property_tree::ptree& t) const
{
	t.put("coefficients", floatingVectToStr<float>(m_coefficients));
	t.put("functionType", (int)m_functionType);
	return true;
}
