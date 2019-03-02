#include "AudioReader.h"

#include <fstream>

AudioReader::AudioReader()
{
}

AudioFile::AudioFile(const std::string & filename)
	:
	m_loaded{false},
	m_data{},
	m_header{},
	m_fileName{filename}
{
}

void AudioFile::reset(const std::string & fileName)
{
	m_data = chunk_t{};
	m_header = wav_header_t{};
	m_fileName = fileName;
	m_loaded = false;
}

bool AudioFile::read()
{
	std::ifstream t_stream{ m_fileName,std::ios::binary };
	t_stream.read((char*)&m_header, sizeof(m_header));

	/*  Then read size of samples right? */


	return false;
}

wav_header_t::wav_header_t()
	: 
	m_audioFormat{0},
	m_bitsPerSample{ 0 },
	m_blockAlign{0},
	m_byteRate{0},
	m_chunkID{'v','o','i','d'},
	m_chunkSize{0},
	m_format{ 'v','o','i','d' },
	m_numChannels{0},
	m_sampleRate{0},
	m_subchunk1ID{ 'v','o','i','d' },
	m_subchunk1Size{0}
{
}

chunk_t::chunk_t()
	: 
	m_id{ 'v','o','i','d' },
	m_size{0}
{
}
