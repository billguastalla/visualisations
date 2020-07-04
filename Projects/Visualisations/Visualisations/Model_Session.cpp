#include "Model_Session.h"
#include <boost/property_tree/ptree.hpp>
#include <fstream>

#include <boost/property_tree/xml_parser.hpp>

using namespace boost::property_tree;

Model_Session::Model_Session()
	:
	m_state{SessionState::Closed},
	m_filename{"test.xml"},
	m_filepath{"C:\\Users\\_\\Desktop\\"},
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
	// prompt to save in window before calling this

	m_state = SessionState::Initialised;
	return false;
}

bool Model_Session::open(std::string file)
{
	// prompt to save in window before calling this
	

	m_state = SessionState::Open;
	return false;
}

bool Model_Session::save()
{
	// todo: check existence of filepath.

	m_fileTree->clear();

	m_fileTree->push_back(ptree::value_type("test", ptree{"123.456"}));
	m_fileTree->put("test.<xmlattr>.id","hello");

	// add all objects/events/mappings here
	write_xml(m_filepath+m_filename,*m_fileTree);

	m_state = SessionState::Open;
	return true;
}

bool Model_Session::close()
{


	m_state = SessionState::Closed;
	return false;
}
