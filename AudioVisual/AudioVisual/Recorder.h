#pragma once
#include "Includes\portaudio\portaudio.h"
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
	LockableBuffer * m_lockedBuffer;
	
	
	int run();

	
	PaStreamParameters  m_inputParameters;
	PaStream*           m_stream;
	PaError             m_err = paNoError;
	//int                 m_i;
	int                 m_totalFrames;
	int                 m_numSamples;
	int                 m_numBytes;
	//SAMPLE              m_max, m_val;
	//double              m_average;
};