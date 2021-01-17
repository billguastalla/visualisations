#pragma once
#include "Buffer.h"
#include <portaudio.h>

class Recorder
{
public:
	Recorder();
	~Recorder();

	enum RecorderState
	{
		Uninitialised,
		Ready,
		Monitoring
	};
	Buffer getBuffer() const;

	RecorderState recorderState() const { return m_state; }
	std::string recorderStateStr() const;

	int sampleRate() const { return m_sampleRate; }
	int framesPerPacket() const { return m_framesPerPacket; }
	int packetsPerBuffer() const { return m_packetsPerBuffer; }
	int channelCount() const { return m_channelCount; }

	bool setSampleRate(const int & sRate);
	bool setFramesPerPacket(const int& fPerPacket);
	bool setPacketsPerBuffer(const int& pPerBuffer);
	bool setChannelCount(const int& cCount);

	PaError startMonitoring();
	PaError stopMonitoring();
	bool reboot(RecorderState rebootToState);

private:
	PaError initialise();
	PaError deinitialise();

	int m_sampleRate;
	int m_framesPerPacket;
	int m_packetsPerBuffer;
	int m_channelCount;

	RecorderState m_state;

	LockableBuffer *    m_lockedBuffer;
	PaStreamParameters  m_inputParameters;
	PaStream *          m_stream;
};
