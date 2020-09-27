#pragma once
#include "AudioTrack.h"
#include <vector>
#include <memory>
#include <map>
#include <set>

using trackID = int;
using propertyID = int;

class AudioTrackSystem
{
public:

	AudioTrackSystem()
		:
		m_tracks{}, m_trackStreams{},
		m_lastTimestep{ Timepoint{},Timepoint{},Timepoint{} },
		m_valueCacheSize{ 512u } // ??
	{};
	// NOTE: Expensive, call only once.
	// TODO:	Use dt in timepoint to look ahead and calculate later timepoints.
	//			mutex/lock for later points.
	std::map<propertyID, double> audioPropertyValues(Timestep ts);

	trackID addTrack(std::string trackName);
	trackID removeTrack(trackID id);
	propertyID registerAudioProperty(const AudioProperty& prop);

	std::set<trackID> trackKeys();
	trackID maxTrack();
	std::set<propertyID> attributeKeys();
	propertyID maxAttribute();

	const std::map<trackID, AudioTrack>& tracks() const { return m_tracks; }
	const std::map<propertyID, AudioProperty>& properties() const { return m_trackAttributes; }
private:
	// TODO: track mutex and cache mutex; caching should interleave locking.
	Timestep m_lastTimestep;
	unsigned m_valueCacheSize; // compare computation time with dt ?

	std::map<trackID, AudioTrack> m_tracks;
	std::map<trackID, std::shared_ptr<std::istream>> m_trackStreams;

	std::map<propertyID, AudioProperty> m_trackAttributes;
};
