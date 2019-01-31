#pragma once
#include <portaudio/portaudio.h>
#include <mutex>
#include <vector>
#include <map>
class Buffer
{
public:
	Buffer(int totalFrames, PaStreamParameters params);

	void loadFrames(int totalFrames, const void * inputBuffer);
	void clear(int frames); // Clear buffer to zeros and resize according to new framecount

	/* Metrics*/
	float amplitude_peak() const;
	float amplitude_average() const;
	float amplitude_minimum() const;

	int channelFrameCount() const { return m_totalFrames / m_channelCount; }
	int channelCount() const { return m_channelCount; }
	std::vector<float> data(int channel) const
	{
		return m_channelData.at(channel);
	}
private:
	PaSampleFormat m_format; // We're only supporting float32's cause we don't need others atm.
	int m_channelCount;
	int m_framesPerChannel;
	int m_totalFrames;

	/* Frames are captured into this: key is channel, 0 for left, 1 for right, value is channel frame data, each -1.0f to 1.0f*/
	std::map<int, std::vector<float>> m_channelData;
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
