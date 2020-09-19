#include "AudioTrack.h"

double AudioTrack::audioPropValue(const Timestep& ts, const AudioProperty& prop)
{
	// TODO: CAST DURATION FROM SECONDS, TO GET OFFSET TIME ts.current() - startTimeSeconds();
	//size_t sampleTime{ ts.current().count() + prop.offset - startTimeSamples()};

	if (prop.channelID < file_channelCount())
		if (prop.type == AudioProperty::AttributeType::Amplitude)
		{
			// TODO: PICK CHANNEL, OR CHANNELS! N.B. AUDIOPROPERTY should have std::set<> channels.
			// calc amplitude..
			// TODO: AudioReader should be able to give you audio data with a stride.
		//	std::vector<float> data{ m_reader.audio(sampleTime,prop.bufsize * (prop.stride+1u)) };
		}
	return 0.0;
}
