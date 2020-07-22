#pragma once
//#include <variant> // TODO: enable C++17

class FunctionSource
{
	// Alternative: (TODO: see comparison between Event & Mapping!!)
	std::map<double, Interpolation> m_keyframes; // set of timed interpolations (interp needs to store initial & final value)

};