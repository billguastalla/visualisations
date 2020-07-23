#include "Model_Session.h"

#include "Program.h"
#include "Model_ViewportSystem.h"
#include "Model_VideoRendering.h"
#include "Model_AudioInterface.h"
#include "Model_Visualisation.h"
#include "Model_Transport.h"

#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace boost::property_tree;

Model_Session::Model_Session(Program & p)
	:
	p_program{p},
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

bool Model_Session::open()
{
	// prompt to save in window before calling this
	m_fileTree->clear();
	read_xml(m_filepath + m_filename, *m_fileTree);

	m_state = SessionState::Open;
	return false;
}

bool Model_Session::save()
{
	// todo: check existence of filepath.
	m_fileTree->clear();
	
	ptree& sessionTree{ m_fileTree->put("visualisations","") };
	sessionTree.add("<xmlattr>.title", m_title);
	sessionTree.add("<xmlattr>.filename", m_filename);
	sessionTree.add("<xmlattr>.filepath", m_filepath);

	//m_fileTree->push_back(ptree::value_type("test", ptree{"123.456"}));
	//m_fileTree->put("test.<xmlattr>.id","hello");

	p_program.modelAudioInterface()->saveFileTree(sessionTree);
	p_program.modelTransport()->saveFileTree(sessionTree);
	p_program.modelVideoRendering()->saveFileTree(sessionTree);
	p_program.modelViewportSystem()->saveFileTree(sessionTree);
	p_program.modelVisualisation()->saveFileTree(sessionTree);

	// add all objects/events/mappings here

	// TODO: Check file is accessible before writing.
	write_xml(m_filepath+m_filename,*m_fileTree);
	write_json(m_filepath+m_filename + ".json", *m_fileTree);
	write_ini(m_filepath+m_filename + ".ini", *m_fileTree);

	m_state = SessionState::Open;
	return true;
}

bool Model_Session::close()
{


	m_state = SessionState::Closed;
	return false;
}
