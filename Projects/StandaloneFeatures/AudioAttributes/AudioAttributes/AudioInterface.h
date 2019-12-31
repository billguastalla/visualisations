#pragma once
#include "AudioFile.h"
#include "AudioTrack.h"
#include <vector>
#include <map>
#include <string>

namespace AudioIO
{
/*
	TODO:
		-> Set and lock sample rate on single file
		-> Add file removal
*/
	class AudioInterface
	{
	public:
		AudioInterface();
		bool addFile(const std::string & fileName = "");

		size_t channelCount();
		unsigned sampleRate() const { return m_sampleRate; }
		std::vector<std::shared_ptr<AudioTrack>> tracks();
		std::vector<std::shared_ptr<AudioChannel>> channels();

		std::shared_ptr<AudioChannel> channel(unsigned trackID, unsigned channelID);
		std::shared_ptr<AudioTrack> track(unsigned trackID);
	private:
		std::map<unsigned /* trackID*/, std::shared_ptr<AudioTrack>> m_tracks;
		unsigned m_sampleRate;
		std::vector<AudioFile> m_files;
	};
}