#pragma once
#include "AudioTrack.h"
#include "Timekeeping.h"
#include <vector>
#include <memory>
#include <map>
#include <set>

struct AudioProperty
{
	AudioProperty(int tID = 0, int cID = 0, size_t bSize = 512u)
		:
		trackID{tID},
		channelID{cID},
		type{AttributeType::Amplitude},
		offset{0}, 
		stride{0},
		bufsize{bSize}
	{}
	int trackID;
	int channelID;
	enum class AttributeType { Amplitude, Pitch /*..*/ } type;
	int offset; // how many samples are we offset from when reading the track?
				// (will be useful for effect where reactions to audio are staggered across objects!)
	size_t stride; // Jump across samples by how much? (n.b. stride of 0 makes read one sample wide)
	size_t bufsize; // how much of the buffer to read to evaluate the property.
};

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
	std::map<int, double> audioPropertyValues(Timestep ts) {}

	bool addTrack(std::string trackName); // TODO: return ids instead, -1 on error.
	bool addAudioProperty(const AudioProperty& prop);

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
