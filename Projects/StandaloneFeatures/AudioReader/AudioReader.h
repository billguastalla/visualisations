#pragma once
#include <vector>
#include <string>
#include <istream>
#include <mutex>
#include <thread>
#include <iostream>

/*
	- The reader IS responsible for loading WAV files into a data structure, and
	handling a cache of buffer data.
	- The reader is NOT responsible for organising the wav data into something understandable
	  by an interface. This is the responisbility of AudioTrack, which contains an instance of an AudioReader.
*/

struct WAV_HEADER
{
	void read(std::istream& s)
	{
		s.read(&m_0_headerChunkID[0], 4);
		s.read((char*)&m_4_chunkSize, 4);
		s.read(&m_8_format[0], 4);
		s.read(&m_12_subchunk1ID[0], 4);

		s.read((char*)&m_16_subchunk1Size, 4);
		s.read((char*)&m_20_audioFormat, 2);
		s.read((char*)&m_22_numChannels, 2);
		s.read((char*)&m_24_sampleRate, 4);

		s.read((char*)&m_28_byteRate, 4);
		s.read((char*)&m_32_bytesPerBlock, 2);
		s.read((char*)&m_34_bitsPerSample, 2);
		s.read(&m_36_dataSubchunkID[0], 4);
		s.read((char*)&m_40_dataSubchunkSize, 4);
	}
	bool valid() const // TODO: Make sure it all checks out & impose constraints here.
	{
		return
			m_20_audioFormat == 1 && // uncompressed
			((m_34_bitsPerSample == 16) ||
				(m_34_bitsPerSample == 24) ||
				(m_34_bitsPerSample == 32)); // 16 or 24-bit
	}

	char m_0_headerChunkID[4];
	int32_t m_4_chunkSize;
	char m_8_format[4];

	int sampleCount() const { return (int)(m_40_dataSubchunkSize / ((m_22_numChannels * m_34_bitsPerSample) / 8)); }

	char m_12_subchunk1ID[4];
	int16_t m_16_subchunk1Size;

	int16_t m_20_audioFormat;
	int16_t m_22_numChannels;
	int32_t m_24_sampleRate;
	int32_t m_28_byteRate;
	int16_t m_32_bytesPerBlock;
	int16_t m_34_bitsPerSample;

	char m_36_dataSubchunkID[4];
	int32_t m_40_dataSubchunkSize;
};

class AudioReader
{
public:
	AudioReader(const std::shared_ptr<std::istream>& stream)
		:
		m_stream{ stream },
		m_header{},
		m_data{},
		m_dataMutex{},
		m_cacheSize{ 2048u }, // 1048576u
		m_cachePos{ 0u },
		m_opened{ false }
	{
	}
	AudioReader(const AudioReader& other)
		:
		m_stream{ other.m_stream },
		m_header{ other.m_header },
		m_data{ other.m_data },
		m_dataMutex{},
		m_cacheSize{ other.m_cacheSize },
		m_cachePos{ other.m_cachePos },
		m_opened{ other.m_opened }
	{

	}

	void reset(const std::shared_ptr<std::istream>& stream)
	{
		std::lock_guard<std::mutex> lock{ m_dataMutex };
		m_stream = stream;
		m_data.clear();
		m_cachePos = 0u;
		m_opened = false;
	}
	bool open()
	{
		if (!m_opened)
		{
			m_header.read(*m_stream.operator->());
			if (m_header.valid())
			{
				m_opened = true;
				load(0u, m_cacheSize);
				return true;
			}
			else
				return false;
		}
		return true; // we didn't open it, but it was already opened.
	}
	void close()
	{
		reset(m_stream);
	}

	// TODO: Add channels by providing offset as a parameter..
	// TODO: Sometimes caller audio() is careful about bounds of load() sometimes it isn't. Load is now made safe, but pick one on caller.
	// NOTE: sample count is the number of samples you would receive with stride 0. so returned std::vector<> size is == samplecount / stride
	std::vector<float> audio(size_t startSample, size_t sampleCount, size_t stride = 0u) // Do we want to guarantee size?
	{
		size_t startSample_ch_bit{ startSample * m_header.m_22_numChannels * (m_header.m_34_bitsPerSample / 8) };
		size_t sampleCount_ch_bit{ sampleCount * m_header.m_22_numChannels * (m_header.m_34_bitsPerSample / 8) };

		if ((startSample_ch_bit + sampleCount_ch_bit) >= (size_t)m_header.m_40_dataSubchunkSize)	// case1: out of bounds of file
		{
			if (startSample_ch_bit >= (size_t)m_header.m_40_dataSubchunkSize)							// case1A: read starts out of bounds
				return std::vector<float>{};
			else																				// case1B: read starts within bounds, ends out of bounds
			{
				load(startSample_ch_bit, m_header.m_40_dataSubchunkSize - startSample_ch_bit);
				return samples(0u, m_header.m_40_dataSubchunkSize - startSample_ch_bit, stride);
			}
		}
		else if (startSample_ch_bit >= m_cachePos &&
			(startSample_ch_bit + sampleCount_ch_bit) <= (m_cachePos + m_data.size()))		// case2: within cache
		{
			std::vector<float> result{ samples(startSample_ch_bit - m_cachePos, sampleCount_ch_bit,stride) };
			if (startSample_ch_bit > (m_cachePos + (m_data.size() / 2)))						// case2A: approaching end of cache
			{
				//std::cout << "\n\t\t*** CALLING EXTEND " << m_cachePos + (m_cacheSize/2) << "-" << (m_cachePos + (3*m_cacheSize/2)) << "***\n";
				std::thread extendBuffer{ &AudioReader::load,this,m_cachePos + (m_cacheSize / 2), m_cacheSize };
				extendBuffer.detach();
			}
			return result;
		}
		else																				// case3: within file, outside of cache
		{
			if (load(startSample_ch_bit, m_cacheSize > sampleCount_ch_bit ? m_cacheSize : sampleCount_ch_bit)) // load samplecount or cachesize, whichever is greater.
				return samples(0u, sampleCount_ch_bit, stride);
			else
				return std::vector<float>{};
		}
	}

	WAV_HEADER header() const { return m_header; }
	std::shared_ptr<std::istream> stream() const { return m_stream; }

private:
	bool load(size_t pos, size_t size) // method will offset read by header size
	{ // TODO: Make this safer & better defined.

		size_t truncatedSize{ (pos + size) < (size_t)m_header.m_40_dataSubchunkSize ? size : (size_t)m_header.m_40_dataSubchunkSize - pos };
		if (pos < m_header.m_40_dataSubchunkSize)
		{
			std::lock_guard<std::mutex> lock{ m_dataMutex };
			m_stream->seekg(((std::streampos)pos + (std::streampos)44u)); // add header size
			m_data.resize(truncatedSize);
			if (m_stream->good())
			{
				m_stream->read((char*)&m_data[0], truncatedSize);
				m_cachePos = pos;
				return true;
			}
		}
		return false;
	}
	std::vector<float> samples(size_t posInCache, size_t size, size_t stride = 0u) // method doesn't know about channels. posInCache is pos relative to cachepos.
	{
		std::vector<float> result{};
		if ((posInCache + size) <= m_data.size())
		{
			std::lock_guard<std::mutex> lock{ m_dataMutex };
			switch (m_header.m_34_bitsPerSample)
			{
			//case 8: // unsigned 8-bit
			//	for (size_t i{ posInCache }; i < size; i += ((m_header.m_34_bitsPerSample / 8u) * (1u + stride)))
			//	{
			//		int16_t v{ (m_data[i] << 8) | m_data[i + 1u] };
			//		float v2{ (float)v / (128.f) }; // TODO: WORK THIS OUT.
			//		result.push_back(v2);
			//	}
			//	break;
			case 16: // 16-bit
				for (size_t i{ posInCache }; i < size; i += ((m_header.m_34_bitsPerSample / 8u) * (1u + stride)))
				{
					int16_t v{ (m_data[i]) | (m_data[i + 1u] << 8) };
					float v2{ (float)v / (32768.f) }; // signed, so divide by 2^15
					result.push_back(v2);
				}
				break;
			case 24: // 24-bit
				for (size_t i{ posInCache }; i < size; i += ((m_header.m_34_bitsPerSample / 8u) * (1u + stride)))
				{
					// 24-bit is different to others: put the value into a 32-bit int with zeros at the (LSB) end
					int32_t v{ (m_data[i] << 8) | (m_data[i + 1u] << 16) | (m_data[i + 2u] << 24) };
					float v2{ (float)v / (2147483648.f) }; // divide by 2^31
					result.push_back(v2);
				}
				break;
			case 32: // 32-bit
				for (size_t i{ posInCache }; i < size; i += ((m_header.m_34_bitsPerSample / 8u) * (1u + stride)))
				{
					int32_t v{ m_data[i] | (m_data[i + 1u] << 8) | (m_data[i + 2u] << 16) | (m_data[i + 3u] << 24) };
					float v2{ (float)v / (2147483648.f) }; // signed, so divide by 2^31
					result.push_back(v2);
				}
				break;
			default:
				break;
			}
		}
		return result;
	}

	bool m_opened;
	std::shared_ptr<std::istream> m_stream;
	WAV_HEADER m_header;

	std::vector<uint8_t> m_data;
	std::mutex m_dataMutex;
	size_t m_cacheSize;
	size_t m_cachePos;
};