#pragma once
#include <GLFW/glfw3.h>
#include <boost/property_tree/ptree_fwd.hpp>

typedef double Timecode; // TODO: switch time information to timestep.
enum class TimeMode { Realtime, Sequence }; // nb window relies on two boolean values for both enums
enum class TransportState { Playing, Paused };

class Model_Transport
{
public:
	bool loadFileTree(const boost::property_tree::ptree& t);
	bool saveFileTree(boost::property_tree::ptree& t) const;


	Model_Transport()
		:
		m_mode{ TimeMode::Sequence },
		m_totalFrames{ 120 },
		m_currentFrame{ 0 },
		m_framerate{ 60 },
		m_state{ TransportState::Paused }
	{}
	// TODO: Switch glfw to C++ chrono.
	Timecode time() { return (m_mode == TimeMode::Sequence) ? (double)m_currentFrame / (double)m_framerate : glfwGetTime(); }
	const TimeMode& mode() { return m_mode; }
	const TransportState& state() { return m_state; }
	std::string stateStr()
	{
		switch (m_state)
		{
		case TransportState::Playing: return "Playing";
		case TransportState::Paused: return "Paused";
		default: return "Unknown";
		}
	}
	std::string modeStr()
	{
		switch (m_mode)
		{
		case TimeMode::Realtime: return "Realtime";
		case TimeMode::Sequence: return "Sequence";
		default: return "Unknown";
		}
	}
	int currentFrame() { return m_currentFrame; }
	int framerate() { return m_framerate; }
	int totalFrames() { return m_totalFrames; }

	void setTransportState(const TransportState& s) { m_state = s; }
	void setTimeMode(const TimeMode& t) { m_mode = t; }
	void setCurrentFrame(int f) { m_currentFrame = f; }
	void setTotalFrames(int t) { m_totalFrames = t; }
	void setFrameRate(int fps) { m_framerate = fps; }

	void nextFrame() { if (m_mode == TimeMode::Sequence && m_state == TransportState::Playing) m_currentFrame++; }
private:
	TransportState m_state;
	TimeMode m_mode;
	int m_totalFrames; // should be unsigned but imgui doesn't support it so not worth the conversion.
	int m_currentFrame;
	int m_framerate;
	//double m_lastFreeTime; // Idea behind this was preventing jumps when switcing between realtime and sequence.
};