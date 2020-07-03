#include "Session.h"
#include "Program.h"

Session::Session(Program & p)
	: p_program{p},
	m_filename{},
	m_sessionMode{SessionMode::Scripted}
{
	
}

void Session::load()
{
}

void Session::save()
{
}
