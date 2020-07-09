#pragma once
#include "CameraSystem.h" // TODO: move interpolation to its own header
#include <boost/property_tree/ptree.hpp> // TODO: Compare perf with fwd decl + pointer type for interpolation.
#include <map>

class Event
{
public:
	Event()
		:
		t_0{ 0. },
		t_f{ 0. },
		m_interp{},
		m_mapTo{},
		m_mapFrom{},
		m_attributes{}
	{}
private:
	double t_0, t_f;
	Interpolation m_interp;
	boost::property_tree::ptree m_mapTo;
	boost::property_tree::ptree m_mapFrom;
	boost::property_tree::ptree m_attributes; // 
	// TODO: Check if there is a lighter alternative to 3 ptrees.

	// Alternative: (TODO: see comparison between Event & Mapping!!)
	std::map<double, Interpolation> m_keyframes; // set of timed interpolations (interp needs to store initial & final value)
};