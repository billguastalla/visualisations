#include "AudioFile.h"
#include <fstream>
using namespace AudioIO;

wav_header_t::wav_header_t()
	:
	m_audioFormat{ 0 },
	m_bitsPerSample{ 0 },
	m_blockAlign{ 0 },
	m_byteRate{ 0 },
	m_chunkID{},
	m_chunkSize{ 0 },
	m_format{},
	m_numChannels{ 0 },
	m_sampleRate{ 0 },
	m_subchunk1ID{},
	m_subchunk1Size{ 0 }
{
}

chunk_t::chunk_t()
	:
	m_id{},
	m_size{ 0 }
{
}

AudioFile::AudioFile(const std::string & filename)
	:
	m_loaded{ false },
	m_data{},
	m_header{},
	m_fileName{ filename }
{
}

void AudioFile::reset(const std::string & fileName)
{
	m_data = chunk_t{};
	m_header = wav_header_t{};
	m_fileName = fileName;
	m_loaded = false;
}

float convert(const char* stream)
{
	int fromStream =
		(0x00 << 24) +
		(stream[2] << 16) +
		(stream[1] << 8) +
		stream[0];
	return (float)fromStream;
}

bool AudioFile::read()
{
	std::ifstream t_stream{ m_fileName,std::ios::binary };
	t_stream.read((char*)&m_header, sizeof(m_header));
	/*  Then read size of samples right? */
	while (true)								// TODO: Fix this. It doesn't work.
	{
		t_stream.read((char*)&m_data, sizeof(m_data));
		if (*(unsigned int *)&m_data.m_id == 0x61746164)
			break;
	}
	unsigned sample_size = m_header.m_bitsPerSample / 8;
	unsigned samples_count = m_data.m_size * 8 / m_header.m_bitsPerSample;

	m_raw.resize(m_data.m_size);
	t_stream.read(&m_raw[0], m_data.m_size);
	//for (unsigned s = 0; s < samples_count; ++s)
	//	t_stream.read((char*)&m_raw[s * sample_size], sample_size);

	/* TODO: Check this.. */
	return true;
}

bool AudioFile::checkFile(const std::string & fileName)
{
	wav_header_t head;
	std::ifstream t_stream{ fileName,std::ios::binary };

	if (t_stream.is_open())
	{
		t_stream.read((char*)&head, sizeof(head));

		std::string formatStr{ head.m_format };
		formatStr.resize(4u);
		std::string subchunkIDStr{ head.m_subchunk1ID };
		subchunkIDStr.resize(3u);
		std::string chunkIDStr{ head.m_chunkID };
		chunkIDStr.resize(4u);

		t_stream.close();

		bool formatAppropriate =
			(
				formatStr == "WAVE" &&
				subchunkIDStr == "fmt" &&
				chunkIDStr == "RIFF"
				);
		return formatAppropriate;
	}
	return false;
}

std::shared_ptr<AudioTrack> AudioIO::AudioFile::buildTrack()
{
	/*
		-> Sample rate
		-> File Name
		-> Samples as vector of float vects
	*/
	//std::vector<std::vector<float>> flValues;
	//int chs = m_header.m_numChannels;
	//flValues.resize(chs);
	unsigned samples_count = m_data.m_size * 8 / m_header.m_bitsPerSample;
	//for (unsigned s = 0; s < samples_count; ++s)
	//{
	//	float val = convert(&m_samples[s]);
	//	flValues[s % chs].push_back(val);
	//}
	//for (unsigned c = 0; c < flValues.size(); ++c) /* Normalise each channel */
	//{
	//	/* 7.86db below 0db, and that converts to ratio of 2.47 ; find numeric limit: */
	//	float max{ 0.0f };
	//	for (auto i = flValues[c].begin(); i != flValues[c].end(); ++i)
	//		if (*i > max)
	//			max = *i;
	//	if (max != 0.0f)
	//		for (auto i = flValues[c].begin(); i != flValues[c].end(); ++i)
	//			*i /= max;
	//}
	float ratio = (float)m_raw.size() / (float)samples_count;


	std::vector<float> values{};
	values.resize(samples_count);
	for (unsigned s = 0; s < samples_count; ++s)
		values[s] = convert(&m_raw[s * 3u]);
	float max{ 0.0f };
	for (auto i = values.begin(); i != values.end(); ++i)
		if (*i > max)
			max = *i;
	if (max != 0.0f)
		for (auto i = values.begin(); i != values.end(); ++i)
			*i /= max;
	std::shared_ptr<AudioTrack> result{ new AudioTrack{m_header.m_sampleRate, m_header.m_numChannels, values} };
	return result;
}
