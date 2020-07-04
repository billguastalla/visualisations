#pragma once
#include <string>
#include <memory>
#include <boost/property_tree/ptree_fwd.hpp>

class Model_Session
{
public:
	Model_Session();

	enum class SessionState
	{
		Closed,			// no file is open
		Initialised,	// new file created, but has never been saved to disk.
		Open,			// file is open, and unchanged
		Modified,		// file is open and has been modified.
	};
	static std::string sessionStateString(const SessionState& s);

	bool create();
	bool open(std::string file);
	bool save();
	bool saveas();
	bool close();

	const SessionState& state() { return m_state; };
	const std::string& title() { return m_title; };
	const std::string& filename() { return m_filename; };
	const std::string& filepath() { return m_filepath; };

	void setTitle(const std::string& t) { m_title = t; };
	void setFilename(const std::string& fn) { m_filename = fn; };
	void setFilepath(const std::string& fp) { m_filepath = fp; };
private:
	std::shared_ptr<boost::property_tree::ptree> m_fileTree;

	SessionState m_state;
	std::string m_title;
	std::string m_filename;
	std::string m_filepath;
};