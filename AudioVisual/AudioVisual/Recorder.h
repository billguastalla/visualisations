#pragma once
#include <portaudio/portaudio.h>
#include "Buffer.h"

class Recorder
{
public:
	Recorder();
	~Recorder();

	void startMonitoring();
	void stopMonitoring();
	Buffer getBuffer() const;
private:
	LockableBuffer *	m_lockedBuffer;
	PaStreamParameters  m_inputParameters;
	PaStream *          m_stream;
	PaError             m_err = paNoError;
};