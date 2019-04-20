#pragma once
#include "AudioTrack.h"

#include <vector>
#include <map>
#include <memory>

namespace AudioIO
{

	class AudioInterface;
	// TODO: Work out how the OutputMuxer should be related to the encoder. 
	class AudioOutputMuxer
	{
	public:
		AudioOutputMuxer(AudioInterface * _interface);

		/* This should be used by the encoder and not the visualisations.
			We always get all channels this way, so we're getting a vector of vectors. */
		std::map<size_t, std::vector<float>> samples(unsigned frameNumber, unsigned fps = 30);

		std::vector<float> samples(int startingSample, int sampleCount);
		int maxSamples();

		struct OutputChannel
		{
			OutputChannel(){}
			struct SourceChannelContribution
			{
				SourceChannelContribution(std::shared_ptr<AudioChannel> & ch, const float & weight)
					: m_channel{ch}, m_weight{ weight }
				{}
				std::shared_ptr<AudioChannel> m_channel;
				float m_weight;
			};
			std::vector<float> samples(int startingSample, int sampleCount);
			std::vector<SourceChannelContribution> m_contributions;
		};
	private:
		// TODO: Set this sample rate by assigning a channel. Then prevent any future assigns
		//		 from including channels of a different sample rate. This should also inform the encoder
		//		 upon setup. (OR remove this, and just use the interface-set sample rate. You're never going to modify the sample rate so why store two values.)
		//unsigned m_sampleRate;
		AudioInterface * p_interface;
		/* Properties needed:
				-> Output Channel Count
				-> OutputChannel:: source channel id list, and for each source channel, a % contribution.
		*/
		std::map<unsigned,OutputChannel> m_outputChannels;
	};
}
