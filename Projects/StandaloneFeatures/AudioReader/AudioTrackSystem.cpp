#include "AudioTrackSystem.h"
#include <fstream>
#include <algorithm>

std::map<int, double> AudioTrackSystem::audioPropertyValues(Timestep ts)
{
	std::map<int, double> result{};

	for (const auto & prop : m_trackAttributes)
	{
		prop.second.trackID;

		
	}

	return result;
}

int AudioTrackSystem::addTrack(std::string trackName)
{
	std::shared_ptr<std::istream> trackStream{ new std::ifstream{trackName,std::ios::binary} };
	if (trackStream->good()) // TODO: Is this the correct check? Why isn't there open()?
	{
		AudioTrack track{ trackStream, trackName };

		int newKey{ maxTrack() + 1 };
		m_tracks.insert(std::pair<int, AudioTrack>{newKey, track}); // TODO: insert probably invalid due to lack of copy/move constructor.
		m_trackStreams.insert(std::pair<int, std::shared_ptr<std::istream>>{newKey, trackStream});

		return newKey;
	}
	else
		return -1;
}

int AudioTrackSystem::registerAudioProperty(const AudioProperty& prop)
{
	// TODO: check CHANNEL TRACK EXIST
	std::set<int> tKeys{ trackKeys() };
	if (tKeys.find(prop.trackID) != tKeys.end())
	{
		if(m_tracks[prop.trackID].file_channelCount() > prop.channelID)
		{
			int propertyID{ maxAttribute() + 1 };
			m_trackAttributes.insert(std::pair<int, AudioProperty>{propertyID, prop});
			return propertyID;
		}
	}
	return -1;
}

std::set<int> AudioTrackSystem::trackKeys()
{
	std::set<int> result{};
	std::transform(m_tracks.begin(), m_tracks.end(),
		std::inserter(result, result.end()),
		[](auto pair) {return pair.first; });
	return result;
}

int AudioTrackSystem::maxTrack()
{
	std::set<int> keys{ trackKeys() };
	if (!keys.empty())
		return *keys.rbegin();
	else
		return 0;
}

std::set<int> AudioTrackSystem::attributeKeys()
{
	std::set<int> result{};
	std::transform(m_trackAttributes.begin(), m_trackAttributes.end(),
		std::inserter(result, result.end()),
		[](auto pair) {return pair.first; });
	return result;
}

int AudioTrackSystem::maxAttribute()
{
	std::set<int> keys{ attributeKeys() };
	if (!keys.empty())
		return *keys.rbegin();
	else
		return 0;
}
