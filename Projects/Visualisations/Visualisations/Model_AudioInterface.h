#pragma once
#include "Buffer.h"
#include "Recorder.h"
#include <memory>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/serialization/access.hpp>

class Settings_AudioInterface;
class Program;

class Model_AudioInterface
{
public:
	bool loadFileTree(const boost::property_tree::ptree& t);
	bool saveFileTree(boost::property_tree::ptree& t) const;


	enum class RecordMode
	{
		AudioStream,
		File
	};

	Model_AudioInterface(Program * prog = nullptr);
	~Model_AudioInterface();

	Buffer buffer();

	bool streamRunning()
	{
		/* Later, we switch over types of streams. */
		return m_realTime->recorderState() == Recorder::RecorderState::Monitoring;
	}
	std::shared_ptr<Recorder>& recorder() { return m_realTime; }
private:
	std::shared_ptr<Recorder> m_realTime;
	//std::shared_ptr<AudioReader> m_audioReader;

	RecordMode m_recordMode;
	Program * p_program;


public:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& m_recordMode;
	}

};

