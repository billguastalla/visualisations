#include "Recorder.h"

#include <stdio.h>
#include <stdlib.h>
#include <portaudio/portaudio.h>

#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (128)
#define NUM_CHANNELS    (2)

/* Select sample format. */
#define PA_SAMPLE_TYPE  paFloat32

static int recordCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
	LockableBuffer * lockedBuffer = (LockableBuffer*)userData;

	Buffer buf{ lockedBuffer->buffer() };
	buf.insertFrames(framesPerBuffer,inputBuffer,timeInfo);
	lockedBuffer->resetBuffer(buf);

	(void)outputBuffer; /* Prevent unused variable warnings. */
	(void)timeInfo;
	(void)statusFlags;
	(void)userData;

	return paContinue;
}

Recorder::Recorder()
	: m_lockedBuffer{ nullptr }
{
	m_err = Pa_Initialize();


	m_inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
	m_inputParameters.channelCount = 2;                    /* stereo input */
	m_inputParameters.sampleFormat = PA_SAMPLE_TYPE;
	m_inputParameters.suggestedLatency = Pa_GetDeviceInfo(m_inputParameters.device)->defaultLowInputLatency;
	m_inputParameters.hostApiSpecificStreamInfo = NULL;

	Buffer buf{ FRAMES_PER_BUFFER * 64,m_inputParameters };
	m_lockedBuffer = new LockableBuffer{ buf };

}

Recorder::~Recorder()
{
	//stopMonitoring();
	Pa_Terminate();
	delete m_lockedBuffer;
}

void Recorder::startMonitoring()
{
	m_err = Pa_OpenStream(
		&m_stream,
		&m_inputParameters,
		NULL,                  /* &outputParameters, */
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,
		paClipOff,      /* we won't output out of range samples so don't bother clipping them */
		recordCallback,
		m_lockedBuffer);
	m_err = Pa_StartStream(m_stream);
}

void Recorder::stopMonitoring()
{
	m_err = Pa_CloseStream(m_stream);
}

Buffer Recorder::getBuffer() const
{
	return m_lockedBuffer->buffer();
}

int Recorder::sampleRate() const
{
	/* If it's flexible later, and it should be, then sample rate can be a variable..  */
	return SAMPLE_RATE;
}