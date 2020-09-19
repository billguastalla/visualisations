#include "AudioTrack.h"

double AudioTrack::audioPropValue(const Timestep& ts, const AudioProperty& prop)
{
	// TODO: CAST DURATION FROM SECONDS, TO GET OFFSET TIME ts.current() - startTimeSeconds();
	size_t ts_SampleTime{ (size_t)std::floor(ts.current().count() * (double)file_sampleRate()) };
	size_t real_sampleTime{ ts_SampleTime + prop.offset - startTimeSamples() };

	double result{ 0.0 };
	if (prop.channelID < file_channelCount())
	{
		switch (prop.type)
		{
		case AudioProperty::AttributeType::Amplitude:
		{
			// TODO: PICK CHANNEL, OR CHANNELS! N.B. AUDIOPROPERTY should have std::set<> channels.
			std::vector<float> data{ m_reader.audio(real_sampleTime,prop.bufsize,prop.stride) };
			for (const auto& s : data)
				result += std::abs(s);
			if (data.size() != 0u)
				result /= (double)data.size();
		}
		case AudioProperty::AttributeType::Pitch:
		{
			// TODO: IMPL PITCH CALCULATION.. 
		}
		default:
		{
		}
		}
	}
	return result;
}
