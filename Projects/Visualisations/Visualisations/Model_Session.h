#pragma once
#include <string>
#include <memory>
#include <boost/serialization/access.hpp>

class Program;
/*
	Entry point for serialisation and the file tree.
*/
class Model_Session
{
public:
	Model_Session(Program& p);

	enum class SessionState
	{
		Closed,			// no file is open
		Initialised,	// new file created, but has never been saved to disk.
		Open,			// file is open, and unchanged
		Modified,		// file is open and has been modified.
	};
	static std::string sessionStateString(const SessionState& s);

	bool create();
	bool open();
	bool save();
	bool close();

	const SessionState& state() { return m_state; };
	const std::string& title() { return m_title; };
	const std::string& filename() { return m_filename; };
	const std::string& filepath() { return m_filepath; };

	void setTitle(const std::string& t) { m_title = t; };
	void setFilename(const std::string& fn) { m_filename = fn; }; // note: I think you only need one of these
	void setFilepath(const std::string& fp) { m_filepath = fp; };
private:
	Program& p_program;

	SessionState m_state;
	std::string m_title;
	std::string m_filename;
	std::string m_filepath;

public:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& m_title;
		ar& *p_program.modelAudioInterface().operator->();
		ar& *p_program.modelTransport().operator->();
		ar& *p_program.modelVideoRendering().operator->();
		ar& *p_program.modelViewportSystem().operator->();
		ar& *p_program.modelVisualisation().operator->();
	}
};