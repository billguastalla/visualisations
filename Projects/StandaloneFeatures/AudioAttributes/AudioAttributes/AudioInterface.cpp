#include "AudioInterface.h"
#include "AudioTrack.h"
using namespace AudioIO;

AudioInterface::AudioInterface()
	:
	m_files{},
	m_sampleRate{ 48000 } /* TODO: Set by adding file. */
{
}

bool AudioInterface::addFile(const std::string & fileName)
{
	if (AudioFile::checkFile(fileName))
	{
		AudioFile file{ fileName };
		if (file.read())
		{
			m_files.push_back(file);
			/* Insert a track, built from the file. */
			m_tracks.insert(std::pair<unsigned, std::shared_ptr<AudioTrack>>(m_tracks.size() != 0u ? m_tracks.rbegin()->first + 1u : 0u, file.buildTrack()));
			return true;
		}
	}
	return false;
}

size_t AudioInterface::channelCount()
{
	size_t chCount{ 0 };
	for (auto tr = m_tracks.begin(); tr != m_tracks.end(); ++tr)
		chCount += tr->second->channelCount();
	return chCount;
}

std::vector<std::shared_ptr<AudioTrack>> AudioIO::AudioInterface::tracks()
{
	std::vector<std::shared_ptr<AudioTrack>> res{};
	for (auto tr{ m_tracks.begin() }; tr != m_tracks.end(); ++tr)
		res.push_back((*tr).second);
	return res;
}

std::vector<std::shared_ptr<AudioChannel>> AudioIO::AudioInterface::channels()
{
	std::vector<std::shared_ptr<AudioChannel>> res{};
	for (std::map<unsigned, std::shared_ptr<AudioTrack>>::iterator tr = m_tracks.begin(); tr != m_tracks.end(); ++tr)
	{
		std::vector<std::shared_ptr<AudioChannel>> trChs{ (*tr).second->channels() };
		res.insert(res.end(), trChs.begin(), trChs.end());
	}
	return res;
}

std::shared_ptr<AudioChannel> AudioIO::AudioInterface::channel(unsigned trackID, unsigned channelID)
{
	std::map<unsigned, std::shared_ptr<AudioTrack>>::iterator tr = m_tracks.find(trackID);
	if (tr != m_tracks.end())
		return tr->second->channel(channelID);
	return std::shared_ptr<AudioChannel>{};
}

std::shared_ptr<AudioTrack> AudioIO::AudioInterface::track(unsigned trackID)
{
	std::map<unsigned,std::shared_ptr<AudioTrack>>::iterator tr = m_tracks.find(trackID);
	if (tr != m_tracks.end())
		return tr->second;
	return std::shared_ptr<AudioTrack>{};
}
