#pragma once
#include "AudioFile.h"
#include "AudioTrack.h"
#include <vector>
#include <map>
#include <string>

namespace AudioIO
{

	class AudioInterface
	{
	public:
		struct AudioChannelLocation
		{
			AudioChannelLocation(size_t trackID, size_t channelID) : m_trackID{ trackID }, m_channelID{ channelID } {};
			size_t m_trackID;
			size_t m_channelID;
		};

		AudioInterface();
		bool addFile(const std::string & fileName = "");

		size_t channelCount();
		std::vector<size_t> channelIDs();

		unsigned sampleRate() const { return m_sampleRate; }

		/*
			-> This should be called by the
			-> Result will always populate a std::vector with bufferSize values.
			-> On failure, function always returns vect of 0.0s.
		*/
		std::vector<float> samples(unsigned trackID, unsigned channelID, unsigned long offset, size_t bufferSize = 512, short bufferOffset = 256);
		std::vector<float> samples(unsigned trackID, unsigned channelID, time_t offset, size_t bufferSize = 512, short bufferOffset = 256);

		std::vector<std::shared_ptr<AudioTrack>> tracks();
		std::vector<std::shared_ptr<AudioChannel>> channels();
	private:
		std::map<unsigned /* Interface ID*/, AudioChannelLocation> m_channels;
		std::map<unsigned /* trackID*/, std::shared_ptr<AudioTrack>> m_tracks;
		unsigned m_sampleRate;

		std::vector<AudioFile> m_files;
	};

}