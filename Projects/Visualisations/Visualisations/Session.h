#pragma once
#include <boost/property_tree/ptree_fwd.hpp>

class Program;

class Session
{
public:
	Session(Program & p);
	void load();
	void save();

private:
	Program & p_program; // you don't own this.

	std::string m_filename;
	enum class SessionMode { Sandbox, Scripted } m_sessionMode;
};