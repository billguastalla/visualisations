#pragma once
#include "AudioTrack.h"
#include <string>
#include <vector>

namespace AudioIO
{
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

	class AudioFile
	{
	public:
		AudioFile(const std::string & filename = "");
		void reset(const std::string & fileName = "");
		bool read();

		static bool checkFile(const std::string & fileName = "");

		unsigned short channels() { return m_header.m_numChannels; }
		//std::vector<float> samples(unsigned channel, unsigned long offset, size_t bufferSize, short bufferOffset);
		//std::vector<float> samples(unsigned channel, time_t offset, size_t bufferSize, short bufferOffset);
		int sampleRate() { return m_header.m_sampleRate; }

		std::shared_ptr<AudioTrack> buildTrack();
	private:
		/* Need max value for each channel. */
		bool m_loaded;
		std::string m_fileName;

		wav_header_t m_header;
		/* Decision: Do you leave chunk_t as it is, or do you turn it into a vector of floats? I suggest you leave it. */
		chunk_t m_data;
		std::vector<char> m_raw;
	};
}
