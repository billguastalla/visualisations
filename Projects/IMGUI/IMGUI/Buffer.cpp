#include "Buffer.h"
#include <math.h>
#include <iostream>

Buffer::Buffer(int totalFrames, PaStreamParameters params)
	:
	m_channelCount{ params.channelCount },
	m_format{ params.sampleFormat },
	m_maxTotalFrames{ totalFrames },
	m_channelData{}
{
	clear(totalFrames);
}

void Buffer::insertFrames(int totalFrames, const void * inputBuffer, const PaStreamCallbackTimeInfo * timeInfo)
{
	const float *rptr = (const float*)inputBuffer;

	/* TODO: Buffer should not be constructed with buffer size not divisible by channel count.
	-> Check this earlier
	*/
	//if (m_totalFrames % m_channelCount != 0)
	//	std::cout << "WARNING: Framesize is not a multiple of channel numbers!!";

	for (int c = 0; c < m_channelCount; ++c)
	{
		std::deque<float> & channelData = m_channelData[c];
		for (int frameIdx = 0; frameIdx < totalFrames; ++frameIdx)
			channelData.push_back( *(rptr + (frameIdx * m_channelCount) + c));

		size_t extraFrames = (channelData.size() - (size_t)maxChannelFrameCount());
		for (size_t frameIdx = 0; frameIdx < extraFrames; ++frameIdx)
			channelData.pop_front();
	}
}

void Buffer::clear(int totalFrameCount)
{
	m_channelData.clear();

	int maxChannelFrames = m_maxTotalFrames / m_channelCount;
	for (int c = 0; c < m_channelCount; ++c)
	{
		std::deque<float> channelIdxFrames{};
		channelIdxFrames.resize(maxChannelFrames);
		for (int f = 0; f < maxChannelFrames; ++f)
			channelIdxFrames[f] = 0.0f;

		std::pair<int, std::deque<float>> channelIdxKeyVal{c,channelIdxFrames};
		m_channelData.insert(channelIdxKeyVal);
	}
}

float Buffer::amplitude_peak() const
{
	float peak{ -1.0f };
	for (int c = 0; c < m_channelCount; ++c)
	{
		const std::deque<float> & chData = m_channelData.at(c);
		for (unsigned int f = 0; f < m_channelData.size(); ++f)
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
		const std::deque<float> & chData = m_channelData.at(c);
		for (unsigned int f = 0; f < m_channelData.size(); ++f)
				sum += chData[f];
	}
	average = sum / (float)m_channelData.size();
	return average;
}

float Buffer::amplitude_minimum() const
{
	float minimum{ 1.0f };
	for (int c = 0; c < m_channelCount; ++c)
	{
		const std::deque<float> & chData = m_channelData.at(c);
		for (unsigned int f = 0; f < m_channelData.size(); ++f)
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
//
//MultiBuffer::MultiBuffer(size_t bufferCount)
//	: m_bufferCount{bufferCount}
//{
//}
//
//void MultiBuffer::pushBuffer(Buffer buf)
//{
//	std::lock_guard<std::mutex> lock{ m_multiBufferMutex };
//	m_buffers.push_back(buf);
//	if(m_buffers.size() > m_bufferCount)
//		m_buffers.pop_front();
//}
//
//std::vector<Buffer> MultiBuffer::getBuffers()
//{
//	std::lock_guard<std::mutex> lock{ m_multiBufferMutex };
//	std::vector<Buffer> result{};
//	for (std::deque<Buffer>::iterator i = m_buffers.begin(); i != m_buffers.end(); ++i)
//		result.push_back(*i);
//	return result;
//}
//
//Buffer MultiBuffer::getConcatenatedBuffer()
//{
//	std::lock_guard<std::mutex> lock{ m_multiBufferMutex };
//	Buffer result{ 0,PaStreamParameters{} };
//	//for (size_t i = 0; i < m_buffers.size(); ++i)
//	//	result.append(m_buffers[i]);
//	return result;
//}
