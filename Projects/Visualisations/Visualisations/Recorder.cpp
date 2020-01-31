#include "Recorder.h"

#include <stdio.h>
#include <stdlib.h>
#include <portaudio/portaudio.h>
#include <imgui/imgui.h>

#define PA_SAMPLE_TYPE  paFloat32

static int recordCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	LockableBuffer* lockedBuffer = (LockableBuffer*)userData;

	Buffer buf{ lockedBuffer->buffer() };
	buf.insertFrames(framesPerBuffer, inputBuffer, timeInfo);
	lockedBuffer->resetBuffer(buf);

	(void)outputBuffer; /* Prevent unused variable warnings. */
	(void)timeInfo;
	(void)statusFlags;
	(void)userData;

	return paContinue;
}

Recorder::Recorder()
	:
	m_state{ RecorderState::Uninitialised },


	m_inputParameters{},
	m_lockedBuffer{ nullptr },
	m_stream{ nullptr },

	m_sampleRate{ 44100 },
	m_framesPerPacket{ 128 },
	m_packetsPerBuffer{ 64 },
	m_channelCount{ 2 }

{
}

Recorder::~Recorder()
{
	deinitialise();
}

bool Recorder::setSampleRate(const int& sRate)
{
	bool fail{ false };
	int previous = m_sampleRate;
	m_sampleRate = sRate;
	if (fail = !reboot(m_state))
		m_sampleRate = previous;
	return !fail;
}

bool Recorder::setFramesPerPacket(const int& fPerPacket)
{
	bool fail{ false };
	int previous = m_framesPerPacket;
	m_framesPerPacket = fPerPacket;
	if (fail = !reboot(m_state))
		m_framesPerPacket = previous;
	return !fail;
}

bool Recorder::setPacketsPerBuffer(const int& pPerBuffer)
{
	bool fail{ false };
	int previous = m_packetsPerBuffer;
	m_packetsPerBuffer = pPerBuffer;
	if (fail = !reboot(m_state))
		m_packetsPerBuffer = previous;
	return !fail;
}

bool Recorder::setChannelCount(const int& cCount)
{
	bool fail{ false };
	int previous = m_channelCount;
	m_channelCount = cCount;
	if (fail = !reboot(m_state))
		m_channelCount = previous;
	return !fail;
}

bool Recorder::reboot(RecorderState rebootToState)
{
	PaError error{ paNoError };
	deinitialise();
	error = initialise();
	if (error != paNoError)
		return false;
	if (rebootToState == RecorderState::Monitoring)
		return (startMonitoring() == paNoError);
	else
		return true;
}

PaError Recorder::initialise()
{
	PaError result{ paNoError };
	if (m_state == RecorderState::Uninitialised)
	{
		result = Pa_Initialize();
		if (result == paNoError)
		{
			m_inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
			m_inputParameters.channelCount = m_channelCount;                    /* stereo input */
			m_inputParameters.sampleFormat = PA_SAMPLE_TYPE;

			const PaDeviceInfo* deviceInfo{ Pa_GetDeviceInfo(m_inputParameters.device) };
			if (deviceInfo == nullptr)
				return paDeviceUnavailable; /* <-- No sound device! */

			m_inputParameters.suggestedLatency = deviceInfo->defaultLowInputLatency;
			m_inputParameters.hostApiSpecificStreamInfo = nullptr;

			Buffer buf{ (size_t)m_framesPerPacket * m_packetsPerBuffer,m_inputParameters };
			m_lockedBuffer = new LockableBuffer{ buf };

			m_state = RecorderState::Ready;
		}
	}
	return result;
}

PaError Recorder::deinitialise()
{
	PaError result{ paNoError };
	result = stopMonitoring();
	if (m_state == RecorderState::Ready)
	{
		result = Pa_Terminate();
		delete m_lockedBuffer;
		m_lockedBuffer = nullptr;
		m_state = RecorderState::Uninitialised;
	}
	return result;
}

PaError Recorder::startMonitoring()
{
	PaError result{ initialise() };
	if (m_state == RecorderState::Ready)
	{
		result = Pa_OpenStream(
			&m_stream,
			&m_inputParameters,
			nullptr,                  /* &outputParameters, */
			m_sampleRate,
			m_framesPerPacket,
			paClipOff,      /* we won't output out of range samples so don't bother clipping them */
			recordCallback,
			m_lockedBuffer);

		if (result == paNoError)
		{
			result = Pa_StartStream(m_stream);
			if (result == paNoError)
				m_state = RecorderState::Monitoring;
			/* Bill: else terminate?*/
		}
	}
	return result;
}

PaError Recorder::stopMonitoring()
{
	PaError result{ paNoError };
	if (m_state == RecorderState::Monitoring)
	{
		result = Pa_CloseStream(m_stream);
		m_state = RecorderState::Ready;
	}
	return result;
}

Buffer Recorder::getBuffer() const
{
	return m_lockedBuffer->buffer();
}

std::string Recorder::recorderStateStr() const
{
	switch (m_state)
	{
	case Recorder::Uninitialised:
		return "Uninitialised";
	case Recorder::Ready:
		return "Ready";
	case Recorder::Monitoring:
		return "Monitoring";
	default:
		return "Unknown";
	}
}

void RecorderUI::draw() const
{
	Recorder::RecorderState state = m_recorder->recorderState();
	bool startClicked{ false }, stopClicked{ false };
	if (state != Recorder::RecorderState::Monitoring)
	{
		startClicked = ImGui::Button("Start");
	}
	else
		stopClicked = ImGui::Button("Stop");
	
	if(startClicked)
		m_recorder->startMonitoring();
	if(stopClicked)
		m_recorder->stopMonitoring();

	std::string status{ "Status: [" };
	status += m_recorder->recorderStateStr();
	status += "]";
	ImGui::Text(status.c_str());

	int channels{ m_recorder->channelCount() };
	int prevChannels{ channels };
	ImGui::SliderInt("Channels", &channels, 1, 2);
	int framesPerPacket = m_recorder->framesPerPacket();
	int prevFramesPerPacket{ framesPerPacket };
	ImGui::SliderInt("Frames Per Packet", &framesPerPacket, 64, 2048);
	int packetsPerBuffer = m_recorder->packetsPerBuffer();
	int prevPacketsPerBuffer{ packetsPerBuffer };
	ImGui::SliderInt("Packets Per Buffer", &packetsPerBuffer, 8, 256);
	if (channels != prevChannels)
		m_recorder->setChannelCount(channels);
	if (framesPerPacket != prevFramesPerPacket)
		m_recorder->setFramesPerPacket(framesPerPacket);
	if (packetsPerBuffer != prevPacketsPerBuffer)
		m_recorder->setPacketsPerBuffer(packetsPerBuffer);
}
