#pragma once
#include <portaudio/portaudio.h>
#include "Buffer.h"
#include <GIST\Gist.h>

class Recorder
{
public:
	Recorder();
	~Recorder();

	void startMonitoring();
	void stopMonitoring();
	Buffer getBuffer() const;

	int sampleRate() const;
private:
	LockableBuffer *	m_lockedBuffer;
	PaStreamParameters  m_inputParameters;
	PaStream *          m_stream;
	PaError             m_err = paNoError;
};