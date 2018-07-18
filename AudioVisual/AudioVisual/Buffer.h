#pragma once
#include "Includes\portaudio\portaudio.h"
#include <mutex>
#include <vector>
class Buffer
{
public:
	Buffer(unsigned long frameCount, PaStreamParameters params);

	void loadFrames(unsigned long frameCount, const void * inputBuffer);
	unsigned long sampleCount() const { return m_channelCount * m_frameCount; }
	void clear(unsigned long frames); // Clear buffer to zeros and resize according to new framecount

	/* Metrics*/
	float amplitude_peak();
	float amplitude_average();
	float amplitude_minimum();

private:
	PaSampleFormat m_format; // We're only supporting float32's cause we don't need others atm.
	int m_channelCount;
	unsigned long m_frameCount;
	std::vector<float> m_data;
};

class LockableBuffer
{
public:
	LockableBuffer(Buffer buf);
	void resetBuffer(Buffer buf);
	Buffer buffer();
private:
	Buffer m_buffer;
	std::mutex m_bufferMutex;
};
