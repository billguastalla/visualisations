#pragma once
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
	void drawUI(const std::string& name);

	// TODO: read/write ptree 
};