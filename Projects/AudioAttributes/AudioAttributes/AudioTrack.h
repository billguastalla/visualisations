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
		std::vector<std::shared_ptr<AudioChannel>> channels() & { return m_channels; }
	private:
		std::vector<std::shared_ptr<AudioChannel>> m_channels;
	};
}
