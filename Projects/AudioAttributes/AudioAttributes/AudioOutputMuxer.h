#pragma once
#include "AudioTrack.h"
#include <vector>
#include <map>
#include <memory>

namespace AudioIO
{
	class AudioInterface;
	/* TODO: Work out how the OutputMuxer should be related to the encoder
			-> Answer: The Output Muxer should not know about the encoder, but the encoder should require the muxer.
					-> This is because there are multiple expected methods to encoding, one method to muxing.
	*/
	class AudioOutputMuxer
	{
	public:
		AudioOutputMuxer(AudioInterface * _interface);

		/* This interleaves samples for all channels, without bounds checking. */
		std::vector<float> samples(int startingSample, int sampleCount);

		/* The length in samples of the shortest track in the output muxer. */
		unsigned minimumSampleCount();
		/* The length in samples of the longest track in the output muxer. 
			All samples retrieved beyond here are 0.0. */
		unsigned maximumSampleCount();

		struct OutputChannel
		{
			OutputChannel(){}
			struct SourceChannelContribution
			{
				SourceChannelContribution(std::shared_ptr<AudioChannel> & ch, const float & weight)
					: m_channel{ch}, m_weight{ weight } {}
				std::shared_ptr<AudioChannel> m_channel;
				float m_weight;
			};
			std::vector<float> samples(int startingSample, int sampleCount);
			std::vector<SourceChannelContribution> m_contributions;
		};
	private:
		std::vector<unsigned> sampleCounts() const;

		AudioInterface * p_interface;
		std::map<unsigned,OutputChannel> m_outputChannels;
	};
}
