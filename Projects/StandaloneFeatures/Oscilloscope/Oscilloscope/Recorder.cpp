#include "Recorder.h"

#include <stdio.h>
#include <stdlib.h>
#include <portaudio/portaudio.h>

#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (8192)
#define NUM_SECONDS     (5)
#define NUM_CHANNELS    (2)

/* Select sample format. */
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)

typedef struct
{
	int          frameIndex;  /* Index into sample array. */
	int          maxFrameIndex;
	SAMPLE      *recordedSamples;
}
paTestData;


static int recordCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
	/*
	billg:
		We're going to:
				-> Write to our lockable data the buffer
		Questions:
				-> 
		Investigate:
				-> What are the other arguments?
	*/
	LockableBuffer * lockedBuffer = (LockableBuffer*)userData;

	Buffer buf{ lockedBuffer->buffer() };
	buf.loadFrames(framesPerBuffer,inputBuffer);
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
	/* Would prefer not to initialise port audio in a class instance. Need a singleton for this probably. */

	m_err = Pa_Initialize();

	m_inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
	m_inputParameters.channelCount = 2;                    /* stereo input */
	m_inputParameters.sampleFormat = PA_SAMPLE_TYPE;
	m_inputParameters.suggestedLatency = Pa_GetDeviceInfo(m_inputParameters.device)->defaultLowInputLatency;
	m_inputParameters.hostApiSpecificStreamInfo = NULL;

	Buffer buf{ 512,m_inputParameters };
	m_lockedBuffer = new LockableBuffer{ buf };

}

Recorder::~Recorder()
{
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
