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

std::map<size_t, std::vector<float>> AudioOutputMuxer::samples(unsigned frameNumber, unsigned fps)
{
	/* TODO: Use the audio interface, along with the channel mappings held in this muxer, to combine the interface channels into this vect of arrays of floats.*/

	std::map<size_t, std::vector<float>> result{};

	unsigned sampleRate = p_interface->sampleRate();
	double dStartingSample = (double)(frameNumber * sampleRate) / fps;
	double dEndingSample = (double)(frameNumber * sampleRate) / fps;

	unsigned uintStartingSample{ (unsigned)std::floor(dStartingSample) };
	unsigned uintEndingSample{ (unsigned)std::floor(dEndingSample) };


	/* Iterate over output channels;*/
	for (auto outputChannel = m_outputChannels.begin();
		outputChannel != m_outputChannels.end();
		++outputChannel)
	{
		std::vector<float> channelData;

		for (auto sourceChannel = outputChannel->second.m_contributions.begin();
			sourceChannel != outputChannel->second.m_contributions.end();
			++sourceChannel)
		{
			/* To build the channel data:
				-> You need to get the sample count from the frame number
				-> You need to get the sample offset from the frame number excluding any offset and starting from the beginning of the samples
				-> You need to check for each input channel first, whether they actually *have*
					the channel data for that time interval.
			*/


		}

		/* Insert the output channel, with the weighted channel data. */
		result.insert(std::pair<size_t, std::vector<float>>{outputChannel->first, channelData});
	}

	return result;
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

int AudioIO::AudioOutputMuxer::maxSamples()
{
	/* TODO: Decide on what maximum means before implementing (after one audio file finishes or after all files finish.. )*/
	return 0;
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
