#include "Model_Session.h"
#include <boost/property_tree/ptree.hpp>
#include <fstream>

#include <boost/property_tree/xml_parser.hpp>

Model_Session::Model_Session()
	:
	m_state{SessionState::Closed},
	m_filename{},
	m_filepath{},
	m_fileTree{ new boost::property_tree::ptree{} }
{
}

std::string Model_Session::sessionStateString(const SessionState& s)
{
	switch (s)
	{
	case Model_Session::SessionState::Closed:
		return "Closed";
	case Model_Session::SessionState::Initialised:
		return "Initialised";
	case Model_Session::SessionState::Open:
		return "Open";
	case Model_Session::SessionState::Modified:
		return "Modified";
	default:
		return "Unknown";
	}
}

bool Model_Session::create()
{
	return false;
}

bool Model_Session::open(std::string file)
{
	return false;
}

bool Model_Session::save()
{
	// todo: check existence of filepath.

	m_fileTree->clear();

	m_fileTree->put("testingtesting", 123);

	// add all objects/events/mappings here
	boost::property_tree::write_xml(m_filepath+m_filename,*m_fileTree);

	m_state = SessionState::Open;
	return true;
}
bool Model_Session::saveas()
{
	return false;
}

bool Model_Session::close()
{
	return false;
}
