#pragma once
#include "CameraSystem.h" // TODO: move interpolation to its own header
#include <boost/property_tree/ptree.hpp> // TODO: Compare perf with fwd decl + pointer type for interpolation.
#include <map>

/*
	1. How are mappings implemented
	2. Options for event:
		-> Does event set value or alter it?
*/
class Event
{
public:
	Event()
		:
		t0_tf{ 0. , 0. },
		m_interp{},
		m_mapTo{},
		m_mapFrom{},
		m_attributes{}
	{}

	bool loadFileTree(const boost::property_tree::ptree& t);
	bool saveFileTree(boost::property_tree::ptree& t) const;
private:
	std::pair<double, double> t0_tf;
	std::pair<double, double> m_inputRange; // do we need this? Should it be placed in interpolation?
	std::pair<double, double> m_outputRange;

	Interpolation m_interp;
	boost::property_tree::ptree m_mapTo; // e.g. particleSystem[1].particleScale
	boost::property_tree::ptree m_mapFrom; // e.g. audiotrack[0].amplitude or transport.timeSeconds
	boost::property_tree::ptree m_attributes; // e.g. method for transition of value. 
	// TODO: Check if there is a lighter alternative to 3 ptrees.

	// Alternative: (TODO: see comparison between Event & Mapping!!)
	std::map<double, Interpolation> m_keyframes; // set of timed interpolations (interp needs to store initial & final value)
};