#pragma once
#include "../time/Timekeeping.h"
#include <GLFW/glfw3.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <string>

enum class TimeMode { Realtime, Sequence }; // nb window relies on two boolean values for both enums
enum class TransportState { Playing, Paused };

class Model_Transport
{
public:
	Model_Transport()
		:
		m_mode{ TimeMode::Sequence },
		m_totalFrames{ 120 },
		m_currentFrame{ 0 },
		m_framerate{ 60 },
		m_state{ TransportState::Paused },

		m_clock{},
		m_prevTime{ std::chrono::steady_clock::now() },
		m_firstTime{ std::chrono::steady_clock::now() },
		m_currentTime{std::chrono::steady_clock::now()}
	{}
	// TODO: Switch glfw to C++ chrono.
	Timestep time()
	{
		return (m_mode == TimeMode::Sequence) ?
			Timestep{
			m_firstTime + Duration{ ((double)m_currentFrame - 1.) / (double)m_framerate },
			m_firstTime + Duration{ (double)m_currentFrame / (double)m_framerate },
			m_firstTime }
			:
			Timestep{ m_prevTime,m_currentTime,m_firstTime };
	}
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

	void nextFrame()
	{
		if (m_mode == TimeMode::Sequence && m_state == TransportState::Playing)
			m_currentFrame++;
		m_prevTime = m_currentTime;
		m_currentTime = m_clock.now();
	}
private:
	std::chrono::steady_clock m_clock;
	const Timepoint m_firstTime;
	Timepoint m_prevTime, m_currentTime;

	TransportState m_state;
	TimeMode m_mode;
	int m_totalFrames; // should be unsigned but imgui doesn't support it so not worth the conversion.
	int m_currentFrame;
	int m_framerate;
	//double m_lastFreeTime; // Idea behind this was preventing jumps when switcing between realtime and sequence.


public:
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
	{
		ar& m_state;
		ar& m_mode;
		ar& m_totalFrames;
		ar& m_currentFrame;
		ar& m_framerate;
	}
	template<class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		ar& m_state;
		ar& m_mode;
		ar& m_totalFrames;
		ar& m_currentFrame;
		ar& m_framerate;

		//m_firstTime = Timepoint{ std::chrono::steady_clock::now() };
		//m_prevTime = Timepoint{ std::chrono::steady_clock::now() };
		//m_currentTime = Timepoint{ std::chrono::steady_clock::now() };
	};
	BOOST_SERIALIZATION_SPLIT_MEMBER()

};