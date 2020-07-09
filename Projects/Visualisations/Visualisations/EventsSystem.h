#pragma once
#include "Event.h"
#include "FunctionSource.h"
#include <map>

class EventsSystem
{
public:
	bool loadFileTree(const boost::property_tree::ptree& t);
	bool saveFileTree(boost::property_tree::ptree& t) const;

	boost::property_tree::ptree buildValueTree();
private:
	std::map<int, Event> m_events;
};