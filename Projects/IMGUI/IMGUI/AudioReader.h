#pragma once
#include "Buffer.h"

#include <vector>
#include <string>

//Wav Header: Taken from: https://gist.github.com/tkaczenko/21ced83c69e30cfbc82b
struct wav_header_t
{
	wav_header_t();

	char m_chunkID[4]; //"RIFF" = 0x46464952
	unsigned long m_chunkSize; //28 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes] + sum(sizeof(chunk.id) + sizeof(chunk.size) + chunk.size)
	char m_format[4]; //"WAVE" = 0x45564157
	char m_subchunk1ID[4]; //"fmt " = 0x20746D66
	unsigned long m_subchunk1Size; //16 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes]
	unsigned short m_audioFormat;
	unsigned short m_numChannels;
	unsigned long m_sampleRate;
	unsigned long m_byteRate;
	unsigned short m_blockAlign;
	unsigned short m_bitsPerSample;
	//[WORD wExtraFormatBytes;]
	//[Extra format bytes]
};

//Chunks
struct chunk_t
{
	chunk_t();
	char m_id[4]; //"data" = 0x61746164
	unsigned long m_size;  //Chunk data bytes
};

/*
	- Features you need:
		1. Have multiple files read simultaneously
		2. Be able to find samples at a timecode for files of different sample rates
		3.
*/

class AudioFile
{
	AudioFile(const std::string & filename = "");
public:
	Buffer getSamples(size_t milliseconds);
	void reset(const std::string & fileName = "");
	bool read();
private:
	bool m_loaded;
	std::string m_fileName;
	wav_header_t m_header;
	chunk_t m_data;
};

class AudioReader
{
public:
	AudioReader();

private:
	std::vector<std::string> m_fileName;
	//std::vector<std::unique_ptr<Buffer>> m_buffers;
};