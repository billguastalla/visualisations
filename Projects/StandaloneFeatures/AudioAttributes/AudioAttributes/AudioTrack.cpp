#include "AudioTrack.h"
using namespace AudioIO;

AudioTrack::AudioTrack(unsigned sampleRate, unsigned channelCount, std::vector<float> samples)
{
	size_t sampleCount = samples.size();
	size_t chSampleCount = sampleCount / channelCount;
	std::vector<std::vector<float>> chSamples;
	chSamples.resize(channelCount);
	for (auto ch = chSamples.rbegin(); ch != chSamples.rend(); ++ch)
		ch->resize(chSampleCount); /* I think reverse iteration is faster because then whole array isn't moved each time we resize elem */
	for (unsigned c{ 0 }; c < channelCount; ++c)
	{
		std::vector<float> & ch{ chSamples[c] };
		for (unsigned chSample{ 0 }; chSample < chSampleCount; ++chSample)
			ch[chSample] = samples[(chSample * channelCount) + c];
	}
	for (unsigned c{ 0 }; c < channelCount; ++c)
		m_channels.push_back(std::shared_ptr<AudioChannel>{new AudioChannel{ chSamples[c],sampleRate }});
}

AudioTrack::~AudioTrack()
{
}

std::shared_ptr<AudioChannel> AudioIO::AudioTrack::channel(unsigned channelID)
{
	if (channelID < m_channels.size() && channelID >= 0)
		return m_channels[channelID];
	return std::shared_ptr<AudioChannel>();
}
