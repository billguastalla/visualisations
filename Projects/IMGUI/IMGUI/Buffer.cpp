#include "Buffer.h"
#include <math.h>
#include <iostream>

Buffer::Buffer(int totalFrames, PaStreamParameters params)
	:
	m_channelCount{ params.channelCount },
	m_format{ params.sampleFormat },
	m_totalFrames{ totalFrames },
	m_framesPerChannel{totalFrames / params.channelCount}
{
	clear(totalFrames);
}

void Buffer::loadFrames(int totalFrames, const void * inputBuffer)
{
	const float *rptr = (const float*)inputBuffer;

	/* TODO: Reconcile framecount (512) with passed sample size (8192)*/
	if (totalFrames != m_totalFrames)
		clear(totalFrames);

	/* TODO: Buffer should not be constructed with buffer size not divisible by channel count.
			-> Check this earlier
	*/
	if (m_totalFrames % m_channelCount != 0)
		std::cout << "WARNING: Framesize is not a multiple of channel numbers!!";

	for (int frameIdx = 0; frameIdx < m_framesPerChannel; ++frameIdx)
	{
		for (int c = 0; c < m_channelCount; ++c)
			m_channelData[c][frameIdx] = *(rptr + (frameIdx * m_channelCount) + c);
	}
}

void Buffer::clear(int totalFrameCount)
{
	m_totalFrames = totalFrameCount;
	m_framesPerChannel = totalFrameCount / m_channelCount;

	m_channelData.clear();
	for (int c = 0; c < m_channelCount; ++c)
	{
		std::vector<float> channelIdxFrames{};
		channelIdxFrames.resize(m_framesPerChannel);
		for (int f = 0; f < m_framesPerChannel; ++f)
			channelIdxFrames[f] = 0.0f;

		std::pair<int, std::vector<float>> channelIdxKeyVal{c,channelIdxFrames};
		m_channelData.insert(channelIdxKeyVal);
	}
}

float Buffer::amplitude_peak() const
{
	float peak{ -1.0f };
	for (int c = 0; c < m_channelCount; ++c)
	{
		const std::vector<float> & chData = m_channelData.at(c);
		for (int f = 0; f < m_framesPerChannel; ++f)
			if (peak < chData[f])
				peak = chData[f];
	}
	return peak;
}

float Buffer::amplitude_average() const
{
	float sum{ 0.0f }, average{ 0.0f };
	for (int c = 0; c < m_channelCount; ++c)
	{
		const std::vector<float> & chData = m_channelData.at(c);
		for (int f = 0; f < m_framesPerChannel; ++f)
				sum += chData[f];
	}
	average = sum / (float)m_totalFrames;
	return average;
}

float Buffer::amplitude_minimum() const
{
	float minimum{ 1.0f };
	for (int c = 0; c < m_channelCount; ++c)
	{
		const std::vector<float> & chData = m_channelData.at(c);
		for (int f = 0; f < m_framesPerChannel; ++f)
			if(minimum > chData[f])
				minimum = chData[f];
	}
	return minimum;
}

LockableBuffer::LockableBuffer(Buffer buf)
	: m_buffer{ buf }
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
