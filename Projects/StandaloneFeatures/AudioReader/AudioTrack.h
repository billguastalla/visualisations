#pragma once
#include "Timekeeping.h"
#include "AudioReader.h"

struct AudioProperty
{
	AudioProperty(int tID = 0, int cID = 0, size_t bSize = 512u)
		:
		trackID{ tID },
		channelID{ cID },
		type{ AttributeType::Amplitude },
		offset{ 0 },
		stride{ 0 },
		bufsize{ bSize }
	{}
	int trackID;
	int channelID;
	enum class AttributeType { Amplitude, Pitch /*..*/ } type;
	int offset; // how many samples are we offset from when reading the track?
				// (will be useful for effect where reactions to audio are staggered across objects!)
	size_t stride; // Jump across samples by how much? (n.b. stride of 0 makes read one sample wide)
	size_t bufsize; // how much of the buffer to read to evaluate the property.
};

class AudioTrack
{
public:
	// TODO: consider pre-normalisation of attributes.
	AudioTrack(const std::shared_ptr<std::istream>& stream = std::shared_ptr<std::istream>{}, std::string trackName = std::string{})
		:
		m_startTimeSamples{0u},
		m_reader{stream},
		m_name{trackName}
	{}
	AudioTrack(const AudioTrack& other)
		:
		m_startTimeSamples{other.m_startTimeSamples},
		m_reader{other.m_reader},
		m_name{other.m_name}
	{
	}

	void setStartTime(double t) {} // ..
	size_t startTimeSamples() { return m_startTimeSamples; }
	double startTimeSeconds() { return 0.0; }

	int file_channelCount() { return m_reader.header().m_22_numChannels; }
	int file_sampleRate() { return m_reader.header().m_24_sampleRate; }
	int file_bps() { return m_reader.header().m_34_bitsPerSample; }
	int file_totalSamplesPerChannel() { return m_reader.header().sampleCount(); }


	double audioPropValue(const Timestep& ts, const AudioProperty& prop);
private:
	AudioReader m_reader;
	// At how many samples from the origin does the track begin.
	size_t m_startTimeSamples; 
	std::string m_name;

};