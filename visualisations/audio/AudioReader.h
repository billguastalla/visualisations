#pragma once

#include "Buffer.h"

#include <string>
#include <vector>
#include <map>

/* Think reader should hold files as expected, but there should be a way to configure
		groups of channels from different files into buffers.

		Buffers need to be able to change channelcount.
		WAV-only is fine for now.
*/

/*
-> On ui control flow:
		-> Live audio vs. recorded.
			-> 
*/
//
//
//namespace Stark /* <-- Adam Stark's AudioFile reader https://github.com/adamstark/AudioFile */
//{
//#include <adamstark/AudioFile/AudioFile.h>
//
//}
//
//class AudioReader
//{
//public:
//	AudioReader()
//		:
//		m_lastIdx{ 0 },
//		m_files{},
//		m_buffer{ 2,2u }
//	{
//	}
//
//	/* Redurns index of file in reader. */
//	int load(const std::string& filename)
//	{
//		Stark::AudioFile<double> f{};
//		if (f.load(filename))
//		{
//			f.printSummary();
//			m_files.insert(std::make_pair(m_lastIdx++, f));
//		}
//	}
//	bool unload()
//	{};
//
//	const Stark::AudioFile<double>& file(int idx) const
//	{
//		return m_files.at(idx);
//	}
//
//	// load file.
//	// unload file.
//	// read buffer: by filename, by index, by id list, all.
//	//				pass: index.
//private:
//	Buffer m_buffer;
//
//	/* might be a map, this. */
//	std::map<int, Stark::AudioFile<double>> m_files;
//	int m_lastIdx;
//};
