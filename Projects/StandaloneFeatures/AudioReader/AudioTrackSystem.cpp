#include "AudioTrackSystem.h"
#include <fstream>
#include <algorithm>

bool AudioTrackSystem::addTrack(std::string trackName)
{
    std::shared_ptr<std::istream> trackStream{ new std::ifstream{trackName,std::ios::binary} };
    if (trackStream->good()) // TODO: Is this the correct check? Why isn't there open()?
    {
        AudioTrack track{ trackStream };

        int newKey{ maxTrack() + 1 };
        m_tracks.insert(std::pair<int, AudioTrack>{newKey, track}); // TODO: insert probably invalid due to lack of copy/move constructor.
        m_trackStreams.insert(std::pair<int, std::shared_ptr<std::istream>>{newKey, trackStream});

        return true;
    }
    else
        return false;
}

bool AudioTrackSystem::addAudioProperty(const AudioProperty& prop)
{
    prop.channelID; // ... check
    prop.trackID;
    return false;
}

std::set<int> AudioTrackSystem::trackKeys()
{
    std::set<int> result{};
    std::transform(m_tracks.begin(),m_tracks.end(),
        std::inserter(result,result.end()),
        [](auto pair) {return pair.first; });
    return result;
}

int AudioTrackSystem::maxTrack()
{
    std::set<int> keys{trackKeys()};
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
