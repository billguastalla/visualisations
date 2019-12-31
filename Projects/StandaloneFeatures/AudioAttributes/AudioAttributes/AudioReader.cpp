#include "AudioReader.h"
#include <fstream>

AudioReader::AudioReader()
	:
	m_files{}
{
}

bool AudioReader::addFile(const std::string & fileName)
{
	if (AudioFile::checkFile(fileName))
	{
		AudioFile file{ fileName };

		file.read();

		m_files.push_back(file);
		return true;
	}
	return false;
}

size_t AudioReader::channelCount()
{
	size_t ch{ 0 };
	for (size_t f = 0; f < m_files.size(); ++f)
		ch += m_files[f].channels();
	return ch;
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

bool AudioFile::read()
{
	std::ifstream t_stream{ m_fileName,std::ios::binary };
	t_stream.read((char*)&m_header, sizeof(m_header));

	/*  Then read size of samples right? */

	while (true)
	{

		
		t_stream.read((char*)&m_data, sizeof(m_data));

		if (*(unsigned int *)&m_data.m_id == 0x61746164)
			break;
	}
	unsigned sample_size = m_header.m_bitsPerSample / 8;
	unsigned samples_count = m_data.m_size * 8 / m_header.m_bitsPerSample;

	std::vector<int32_t> values;
	values.resize(samples_count, 0.0);
	for(unsigned s = 0; s < samples_count; ++s)
		t_stream.read((char*)&values[s], sample_size);


	return false;
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
