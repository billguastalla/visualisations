#pragma once
#include <memory>
#include <vector>

namespace AudioIO
{
	class AudioChannel
	{
	public:
		AudioChannel(const std::vector<float> & samples, unsigned sampleRate)
			: m_samples{samples}, m_sampleRate{sampleRate} {}
		const std::vector<float> & samples() const { return m_samples; }
	private:
		std::vector<float> m_samples;
		unsigned m_sampleRate;
	};

	class AudioTrack
	{
	public:
		AudioTrack(unsigned sampleRate, unsigned channelCount, std::vector<float> samples);
		~AudioTrack();
		std::shared_ptr<AudioChannel> channel(unsigned channelID);
		std::vector<std::shared_ptr<AudioChannel>> channels() & { return m_channels; }
		size_t channelCount() { return m_channels.size(); }
	private:
		/* If we're making some sort of persistent system, then either:	
			-> this should also be a map<unsigned,AudioChan..> or/
			-> you should not be able to read channels non-const
				(-> you should never be able to remove a channel)*/
		std::vector<std::shared_ptr<AudioChannel>> m_channels;
	};
}
