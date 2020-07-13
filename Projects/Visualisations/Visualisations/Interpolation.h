#pragma once
#include <boost/property_tree/ptree_fwd.hpp>
#include <string>
#include <vector>

struct Interpolation // TODO: Allow different coefficient sizes in UI. // NOTE: This is a misnomer, it's an R1->R1 function.
{
	enum class FunctionType { Polynomial, Exponential, Sinusoidal };
	Interpolation() : m_functionType{ FunctionType::Polynomial }, m_coefficients{ {0.f,1.f,0.f,0.f} } {}
	// TODO: Fix duplicated code, do more rigorous analysis..
	float value(float x); // expects x between [0,1], undefined behaviour outside.
	FunctionType m_functionType;
	std::vector<float> m_coefficients;

	// TODO: read/write ptree 
	bool loadFileTree(const boost::property_tree::ptree& t);
	bool saveFileTree(boost::property_tree::ptree& t) const;
	void drawUI(const std::string& name);


	std::pair<double, double> m_inputRange;
	std::pair<double, double> m_outputRange;
};