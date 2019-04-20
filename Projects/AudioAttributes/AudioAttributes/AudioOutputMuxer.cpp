#include "AudioOutputMuxer.h"
#include "AudioInterface.h"
#include <stack>
using namespace AudioIO;

AudioOutputMuxer::AudioOutputMuxer(AudioInterface * _interface)
	: p_interface{ _interface }
{
	/* You want to:
		-> create two output channels,
		-> for each ith output channel:
			-> for each jth input file, take the ith input channel of the file, and map it to the ith output channel. */
	m_outputChannels.insert(std::pair<unsigned, OutputChannel>(0u, OutputChannel{}));
	m_outputChannels.insert(std::pair<unsigned, OutputChannel>(1u, OutputChannel{}));

	std::vector<std::shared_ptr<AudioTrack>> tracks{ p_interface->tracks() };

	unsigned channelComponentCount{ 0 };
	for (auto tr = tracks.begin(); tr != tracks.end(); ++tr)
	{
		std::vector<std::shared_ptr<AudioChannel>> trackChannels{ (*tr)->channels() };
		switch (trackChannels.size())
		{
		case 1u:		/* Mono: Add to both LHS & RHS*/
			m_outputChannels[0u].m_contributions.push_back(OutputChannel::SourceChannelContribution{ trackChannels[0],0.0 });
			m_outputChannels[1u].m_contributions.push_back(OutputChannel::SourceChannelContribution{ trackChannels[0],0.0 });
			++channelComponentCount;
			break;
		case 2u:		/* Stereo: Add channels to lhs and rhs respectively.*/
			m_outputChannels[0u].m_contributions.push_back(OutputChannel::SourceChannelContribution{ trackChannels[0],0.0 });
			m_outputChannels[1u].m_contributions.push_back(OutputChannel::SourceChannelContribution{ trackChannels[1],0.0 });
			++channelComponentCount;
			break;
		default:
			break;
		}
	}
	/* Weight contributions evenly across channels (delegate to some function inside outputchannel maybe?)*/
	float contrib = 1 / (float)channelComponentCount;
	for (unsigned chan = 0; chan < 2; ++chan)
		for (unsigned comp = 0; comp < channelComponentCount; ++comp)
			m_outputChannels[chan].m_contributions[comp].m_weight = contrib;

}

std::vector<float> AudioIO::AudioOutputMuxer::samples(int startingSample, int sampleCount)
{
	std::vector<float> result{};
	std::vector<std::vector<float>> channels;
	for(auto ch = m_outputChannels.begin(); ch != m_outputChannels.end(); ++ch)
		channels.push_back((ch->second).samples(startingSample, sampleCount));
	for (int i = 0; i < sampleCount; ++i)
		for (auto ch = channels.begin(); ch != channels.end(); ++ch)
			result.push_back((*ch)[i]);
	return result;
}


unsigned AudioIO::AudioOutputMuxer::minimumSampleCount()
{
	std::vector<unsigned> scounts{ sampleCounts() };
	if (scounts.size() != 0)
	{
		unsigned min = *scounts.begin();
		for (auto sc = scounts.begin(); sc != scounts.end(); ++sc)
			if (min > *sc)
				min = *sc;
		return min;
	}
	return 0;
}

unsigned AudioIO::AudioOutputMuxer::maximumSampleCount()
{
	std::vector<unsigned> scounts{ sampleCounts() };
	for (auto outputIter = m_outputChannels.begin(); outputIter != m_outputChannels.end(); ++outputIter)
		for (auto contribIter = outputIter->second.m_contributions.begin(); contribIter != outputIter->second.m_contributions.end(); ++contribIter)
			scounts.push_back(contribIter->m_channel->sampleCount());
	if (scounts.size() != 0)
	{
		unsigned max = *scounts.begin();
		for (auto sc = scounts.begin(); sc != scounts.end(); ++sc)
			if (max < *sc)
				max = *sc;
		return max;
	}
	return 0;
}

std::vector<unsigned> AudioIO::AudioOutputMuxer::sampleCounts() const
{
	std::vector<unsigned> result{};
	for (auto outputIter = m_outputChannels.begin(); outputIter != m_outputChannels.end(); ++outputIter)
		for (auto contribIter = outputIter->second.m_contributions.begin(); contribIter != outputIter->second.m_contributions.end(); ++contribIter)
			result.push_back(contribIter->m_channel->sampleCount());
	return result;
}

std::vector<float> AudioIO::AudioOutputMuxer::OutputChannel::samples(int startingSample, int sampleCount)
{
	std::vector<float> result{};
	result.resize(sampleCount);
	int endSample{ startingSample + sampleCount };
	for (auto ch = m_contributions.begin(); ch != m_contributions.end(); ++ch)
	{
		const std::vector<float> & chTotal = ch->m_channel->samples();

		if (endSample < chTotal.size()) /* Are we within bounds for the channel.. */
			for (int s = startingSample; s < endSample; ++s)
				result[s - startingSample] += ch->m_weight * chTotal[s];
	}
	return result;
}
