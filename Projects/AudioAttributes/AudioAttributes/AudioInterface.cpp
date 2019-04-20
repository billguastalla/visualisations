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
			size_t chcount{ file.channels() };
			size_t fcount{ m_files.size() };
			for (unsigned c = 0; c < chcount; ++c)
				m_channels.insert(std::pair<unsigned, AudioChannelLocation>(m_channels.size() != 0u ? m_channels.rbegin()->first + 1 : 0u, AudioChannelLocation{ fcount, c }));
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
	size_t ch{ 0 };
	for (size_t f = 0; f < m_files.size(); ++f)
		ch += m_files[f].channels();
	return ch;
}

std::vector<size_t> AudioInterface::channelIDs()
{
	std::vector<size_t> result{};
	for (size_t f = 0; f < m_files.size(); ++f)
		for (size_t c = 0; c < m_files[f].channels(); ++c)
			result.push_back(f + c);
	return result;
}

std::vector<float> AudioInterface::samples(unsigned trackID, unsigned channelID, unsigned long offset, size_t bufferSize, short bufferOffset)
{
	//auto i = m_channels.find(interfaceChannelID);
	//if (i != m_channels.end())
	//{
	//	AudioChannelLocation & loc{ i->second };
	//	if (m_files.size() > loc.m_fileID)/* Can't delete a file here.. Kind of what you wanted before.. */
	//	{
	//		AudioFile & file{ m_files[loc.m_fileID] };
	//		std::vector<float> result{};
	//		/* File should also responsible for bounds checking channel.. */
	//		if (file.channels() > loc.m_channelID);
	//		//result = m_files[loc.m_fileID].samples(loc.m_channelID, offset, bufferSize, bufferOffset);
	//		return result;
	//	}
	//}
	return std::vector<float>{};
}

std::vector<float> AudioInterface::samples(unsigned trackID, unsigned channelID, time_t offset, size_t bufferSize, short bufferOffset)
{
	return std::vector<float>();
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
