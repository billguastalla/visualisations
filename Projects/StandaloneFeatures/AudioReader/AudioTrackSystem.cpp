#include "AudioTrackSystem.h"
#include <fstream>
#include <algorithm>

std::map<propertyID, double> AudioTrackSystem::audioPropertyValues(Timestep ts)
{
	m_lastTimestep = ts;
	std::map<propertyID, double> result{};
	for (const auto& prop : m_trackAttributes) // we don't check trackID exists, because we should have ensured this already.
		result.insert(std::pair<propertyID, double>{prop.first, m_tracks[prop.second.trackID].audioPropValue(ts, prop.second)});
	return result;
}

trackID AudioTrackSystem::addTrack(std::string trackName)
{
	std::shared_ptr<std::istream> trackStream{ new std::ifstream{trackName,std::ios::binary} };
	if (trackStream->good())
	{
		AudioTrack track{ trackStream, trackName };
		if (track.open())
		{
			int newKey{ maxTrack() + 1 };
			m_tracks.insert(std::pair<trackID, AudioTrack>{newKey, track});
			m_trackStreams.insert(std::pair<trackID, std::shared_ptr<std::istream>>{newKey, trackStream});

			return newKey;
		}
	}
	return -1;
}

trackID AudioTrackSystem::removeTrack(trackID id)
{
	if (m_tracks.find(id) != m_tracks.end() && m_trackStreams.find(id) != m_trackStreams.end())
	{
		m_tracks.erase(id);
		m_trackStreams.erase(id);
		return maxTrack();
	}
	else
		return -1;
}

propertyID AudioTrackSystem::registerAudioProperty(const AudioProperty& prop)
{
	std::set<trackID> tKeys{ trackKeys() };
	if (tKeys.find(prop.trackID) != tKeys.end())
	{
		//if (*prop.channelIDs.rbegin() < m_tracks[prop.trackID].file_channelCount())
		{
			propertyID pID{ maxAttribute() + 1 };
			m_trackAttributes.insert(std::pair<propertyID, AudioProperty>{pID, prop});
			return pID;
		}
	}
	return -1;
}

std::set<trackID> AudioTrackSystem::trackKeys()
{
	std::set<trackID> result{};
	std::transform(m_tracks.begin(), m_tracks.end(),
		std::inserter(result, result.end()),
		[](auto pair) {return pair.first; });
	return result;
}

trackID AudioTrackSystem::maxTrack()
{
	std::set<trackID> keys{ trackKeys() };
	if (!keys.empty())
		return *keys.rbegin();
	else
		return -1; // so that tracks are zero-indexed
}

std::set<propertyID> AudioTrackSystem::attributeKeys()
{
	std::set<propertyID> result{};
	std::transform(m_trackAttributes.begin(), m_trackAttributes.end(),
		std::inserter(result, result.end()),
		[](auto pair) {return pair.first; });
	return result;
}

propertyID AudioTrackSystem::maxAttribute()
{
	std::set<propertyID> keys{ attributeKeys() };
	if (!keys.empty())
		return *keys.rbegin();
	else
		return -1;// so that audioproperties are zero-indexed
}
