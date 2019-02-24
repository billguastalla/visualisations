#pragma once
#include <portaudio/portaudio.h>
#include <mutex>
#include <vector>
#include <queue>
#include <map>

#include "kissfft\kiss_fft.h"
#include <complex>

class Buffer
{
public:
	Buffer(size_t totalFrames, PaStreamParameters params);

	void insertFrames(int totalFrames, const void * inputBuffer, const PaStreamCallbackTimeInfo * timeInfo);
	void clear(int frames); // Clear buffer to zeros and resize according to new framecount

	/* Metrics*/
	float amplitude_peak() const;
	float amplitude_average() const;
	float amplitude_minimum() const;

	std::vector<std::vector<kiss_fft_cpx>> fft() const;
	std::vector<float> signalPower() const;
	static void normaliseFFT(std::vector<std::vector<kiss_fft_cpx>> & fftData);

	size_t maxChannelFrameCount() const { return m_maxTotalFrames / m_channelCount; }
	size_t channelCount() const { return m_channelCount; }
	std::deque<float> data(int channel) const
	{
		return m_channelData.at(channel);
	}
	std::map<int, std::deque<float>> data() const
	{
		return m_channelData;
	}
private:
	PaSampleFormat m_format; // We're only supporting float32's cause we don't need others atm.

	size_t m_channelCount; // The number of channels in the buffer
	size_t m_maxTotalFrames; // The maximum number of samples in the buffer

	/* Frames are captured into this: key is channel, 0 for left, 1 for right, value is channel frame data, each -1.0f to 1.0f*/
	/* Why did you choose a map & not a vector? Justify this. Is lookup faster? Surely it's easier
		to compiler-optimise contiguous storage*/
	std::map<int, std::deque<float>> m_channelData;
};

class LockableBuffer
{
public:
	LockableBuffer(Buffer buf);
	void resetBuffer(Buffer buf);
	Buffer buffer();
protected:
	Buffer m_buffer;
	std::mutex m_bufferMutex;
};

//class MultiBuffer
//{
//public:
//	MultiBuffer(size_t bufferCount);
//
//	/* Some things a multibuffer might know about: */
//	void pushBuffer(Buffer buf);
//
//	std::vector<Buffer> getBuffers();
//	Buffer getConcatenatedBuffer();
//private:
//	std::deque<Buffer> m_buffers;
//	size_t m_bufferCount;
//	std::mutex m_multiBufferMutex;
//
//	int m_timeTotal;
//	int m_sampleRate;
//};