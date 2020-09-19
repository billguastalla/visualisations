#pragma once
#include "AudioReader.h"

class AudioTrack
{
public:
	// TODO: consider pre-normalisation of attributes.
	AudioTrack(const std::shared_ptr<std::istream>& stream)
		:
		m_startTimeSamples{0u},
		m_reader{stream}
	{}
	AudioTrack(const AudioTrack& other)
		:
		m_startTimeSamples{other.m_startTimeSamples},
		m_reader{other.m_reader}
	{

	}

	void setStartTime(double t) {} // ..
	size_t startTimeSamples() { return m_startTimeSamples; }
	double startTimeSeconds() { return 0.0; }

	AudioReader m_reader;
private:
	// At how many samples from the origin does the track begin.
	size_t m_startTimeSamples; 
	


};