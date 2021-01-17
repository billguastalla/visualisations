#pragma once
#include "../camera/CameraSystem.h" // TODO: move interpolation to its own header
#include <boost/property_tree/ptree.hpp> // TODO: Compare perf with fwd decl + pointer type for interpolation.
#include <boost/serialization/access.hpp>
#include <map>

/*
	1. How are mappings implemented
	2. Options for event:
		-> Does event set value or alter it?
			-> if it alters it (additively or multiplicatively), the event
				will need to hold the original value.
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
		m_attributes{},
		m_mode{Mode::Assigned}
	{}

	enum class Mode { Assigned, Multiplicative, Additive };
private:
	std::pair<double, double> t0_tf;
	Interpolation m_interp;
	Mode m_mode;

	boost::property_tree::ptree m_mapTo; // e.g. particleSystem[1].particleScale
	boost::property_tree::ptree m_mapFrom; // e.g. audiotrack[0].amplitude or transport.timeSeconds or normalisedEventTime (i.e. (t - t0)/(tf - t0))
											//		could be blank for normal time-based interp.
	boost::property_tree::ptree m_attributes;
	// Modes: {Assigned,Multiplicative,Additive}

	// e.g. method for transition of value or higher-dim value, quaternion/position.
	// TODO: Check if there is a lighter alternative to 3 ptrees.



public:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		// TODO: serialise
	}

};

