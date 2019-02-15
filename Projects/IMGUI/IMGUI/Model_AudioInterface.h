#pragma once

#include "Recorder.h"
#include "Buffer.h"

#include <memory>

class Settings_AudioInterface;
class Program;

class Model_AudioInterface
{
public:
	enum class RecordState
	{
		Started,
		Paused,
		Stopped
	};
	enum class RecordMode
	{
		AudioSteam,
		File
	};

	Model_AudioInterface(std::shared_ptr<Settings_AudioInterface> & settings, Program * prog = nullptr);
	~Model_AudioInterface();

	void startStream();
	void stopStream();

	const std::shared_ptr<Settings_AudioInterface> settings() const { return m_settings; }
	Buffer buffer();

	RecordState state() { return m_recordState; }
private:
	std::shared_ptr<Settings_AudioInterface> m_settings;
	std::shared_ptr<Recorder> m_realTime;

	RecordState m_recordState;
	RecordMode m_recordMode;

	Program * p_program;
};

