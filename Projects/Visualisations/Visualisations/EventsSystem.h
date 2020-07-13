#pragma once
#include "Event.h"
#include "FunctionSource.h"
#include <map>

/*
	1. Who is responsible for setting values?
		-> A function for each object setEventValues() ?

	2. Need a system to see the list of objects which can be mapped to and from,
		with their addresses, so that it is easy to write events up.

	3. At end of deserialisation & on changes to events, need to validate
		the events to confirm all mappings exist.
		-> e.g object function: bool mappingExists(std::string m)

	4. 
*/
class EventsSystem
{
public:
	bool loadFileTree(const boost::property_tree::ptree& t);
	bool saveFileTree(boost::property_tree::ptree& t) const;

	bool validateConnections(std::string & log); // Check that events have mappings and sources, logging issues.

	boost::property_tree::ptree buildValueTree() const;
private:
	mutable boost::property_tree::ptree valueTree;

	std::map<int, Event> m_events;
	std::map<int, FunctionSource> m_mappings;
};