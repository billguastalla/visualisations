#include "Model_Session.h"

#include "Program.h"
#include "Model_ViewportSystem.h"
#include "Model_VideoRendering.h"
#include "Model_AudioInterface.h"
#include "Model_Visualisation.h"
#include "Model_Transport.h"

#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

using namespace boost::property_tree;

Model_Session::Model_Session(Program & p)
	:
	p_program{p},
	m_state{SessionState::Closed},
	m_filename{"session.vis"},
	m_filepath{"C:\\Users\\_\\Desktop\\"}
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
	bool result{ true };
	std::ifstream ifs{ m_filepath + m_filename };
	boost::archive::text_iarchive ia{ifs};
	ia >> *this;
	m_state = SessionState::Open;
	return result;
}

bool Model_Session::save()
{
	std::ofstream ifs{m_filepath + m_filename};
	boost::archive::text_oarchive oa{ ifs };
	oa << *this;
	m_state = SessionState::Open;
	return true;
}

bool Model_Session::close()
{


	m_state = SessionState::Closed;
	return false;
}
