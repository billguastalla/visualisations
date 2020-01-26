#pragma once
#include "Buffer.h"
#include "Recorder.h"
#include <memory>

class Settings_AudioInterface;
class Program;

class Model_AudioInterface
{
public:
	enum class RecordMode
	{
		AudioStream,
		File
	};

	Model_AudioInterface(std::shared_ptr<Settings_AudioInterface> & settings, Program * prog = nullptr);
	~Model_AudioInterface();

	const std::shared_ptr<Settings_AudioInterface> settings() const { return m_settings; }
	Buffer buffer();

	bool streamRunning()
	{
		/* Later, we switch over types of streams. */
		return m_realTime->recorderState() == Recorder::RecorderState::Monitoring;
	}
	std::shared_ptr<Recorder>& recorder() { return m_realTime; }
private:
	std::shared_ptr<Settings_AudioInterface> m_settings;

	std::shared_ptr<Recorder> m_realTime;
	//std::shared_ptr<AudioReader> m_audioReader;

	RecordMode m_recordMode;
	Program * p_program;
};

