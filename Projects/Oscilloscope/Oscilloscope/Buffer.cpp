#include "Buffer.h"

Buffer::Buffer(unsigned long frameCount, PaStreamParameters params)
	:
	m_channelCount{params.channelCount},
	m_format{params.sampleFormat},
	m_frameCount{frameCount}
{
	clear(frameCount);
}

void Buffer::loadFrames(unsigned long framesPerBuffer, const void * inputBuffer)
{
	const float *rptr = (const float*)inputBuffer;
	
	if (framesPerBuffer != m_frameCount)
		clear(framesPerBuffer);

	unsigned long sampleCt = sampleCount();
	int channel = 0;
	for (unsigned long sampleIdx = 0; sampleIdx < sampleCt; sampleIdx++)
	{
		channel = sampleIdx % m_channelCount;

		m_data[sampleIdx] = *rptr;
		*rptr++;
	}
}

void Buffer::clear(unsigned long frames)
{
	m_frameCount = frames;
	unsigned long samples = sampleCount();
	m_data.resize(samples);
	for (unsigned long i = 0; i < samples; ++i)
		m_data[i] = 0.0f;
}

float Buffer::amplitude_peak()
{
	float peak{ -5.0f };
	for (size_t i = 0; i < m_data.size(); ++i)
		if (m_data[i] > peak)
			peak = m_data[i];
	return peak;
}

float Buffer::amplitude_average()
{
	float sum{ 0.0f };
	for (size_t i = 0; i < m_data.size(); ++i)
			sum += m_data[i];
	return sum/(float)m_data.size();
}

float Buffer::amplitude_minimum()
{
	float minimum{ 0.0f };
	for (size_t i = 0; i < m_data.size(); ++i)
		if (m_data[i] < minimum)
			minimum = m_data[i];
	return minimum;
}

LockableBuffer::LockableBuffer(Buffer buf)
	: m_buffer{buf}
{
}

void LockableBuffer::resetBuffer(Buffer buf)
{
	std::lock_guard<std::mutex> lock{ m_bufferMutex };
	m_buffer = buf;
}

Buffer LockableBuffer::buffer()
{
	std::lock_guard<std::mutex> lock{ m_bufferMutex };
	return m_buffer;
}
