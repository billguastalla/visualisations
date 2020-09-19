#include "AudioTrack.h"

double AudioTrack::audioPropValue(const Timestep& ts, const AudioProperty& prop)
{
	// TODO: CAST DURATION FROM SECONDS, TO GET OFFSET TIME ts.current() - startTimeSeconds();
	size_t sampleTime{ ts.current().count() + prop.offset - startTimeSamples() };
	double result{ 0.0 };
	if (prop.channelID < file_channelCount())
		switch (prop.type)
		{
		case AudioProperty::AttributeType::Amplitude:
		{
			// TODO: PICK CHANNEL, OR CHANNELS! N.B. AUDIOPROPERTY should have std::set<> channels.
			std::vector<float> data{ m_reader.audio(sampleTime,prop.bufsize,prop.stride) };
			for (const auto& s : data)
				result += std::abs(s);
			result /= (double)data.size();
		}
		case AudioProperty::AttributeType::Pitch:
		default:
		}
	return result;
}
