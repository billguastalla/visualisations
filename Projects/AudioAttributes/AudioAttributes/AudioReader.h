#pragma once
#include "AudioFile.h"

#include <vector>
#include <string>

class AudioReader
{
public:
	AudioReader();

	bool addFile(const std::string & fileName = "");

	size_t channelCount();
private:
	std::vector<AudioFile> m_files;
};