#pragma once
#include "AudioTrack.h"
#include <vector>
#include <memory>
#include <map>
#include <set>
class AudioTrackSystem
{
public:
	AudioTrackSystem()
		:
		m_tracks{}, m_trackStreams{},
		m_lastTimestep{ Timepoint{},Timepoint{},Timepoint{} },
		m_valueCacheSize{512u} // ??
	{};
	// TODO: Register a property to calculate

	// TODO: Return a tree at a certain time point of all properties.
	//		-> Why return a tree? Why not just give a property an id?


	// NOTE: Expensive, call only once.
	// TODO:	Use dt in timepoint to look ahead and calculate later timepoints.
	//			mutex/lock for later points.
	std::map<int, double> audioPropertyValues(Timestep ts);

	int addTrack(std::string trackName);
	int registerAudioProperty(const AudioProperty& prop);

	std::set<int> trackKeys();
	int maxTrack();
	std::set<int> attributeKeys();
	int maxAttribute();
private:
	// TODO: track mutex and cache mutex; caching should interleave locking.
	Timestep m_lastTimestep;
	unsigned m_valueCacheSize; // compare computation time with dt ?

	std::map<int,AudioTrack> m_tracks;
	std::map<int,std::shared_ptr<std::istream>> m_trackStreams;

	std::map<int, AudioProperty> m_trackAttributes;
};
