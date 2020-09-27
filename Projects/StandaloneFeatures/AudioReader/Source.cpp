#include "AudioTrackSystem.h"
#include "Timekeeping.h"
#include <iostream>
#include <fstream>
int main()
{
	
	AudioTrackSystem tSystem{};
	//tSystem.addTrack("200426_0165.wav");
	tSystem.addTrack("32-bitSine.wav");
	//tSystem.addTrack("200913_0209.wav");
	//tSystem.addTrack("invalidName");

	AudioProperty p1;
	p1.trackID = 0;
	p1.channelID = 0;
	p1.bufsize = 128;
	p1.offset = 0;
	p1.stride = 0;
	p1.type = AudioProperty::AttributeType::Amplitude;
	tSystem.registerAudioProperty(p1); // VALID ATTRIBUTE
	//p1.trackID = 2; 
	//tSystem.registerAudioProperty(p1); // INVALID TRACK
	//p1.trackID = 1;
	//p1.channelID = 4;
	//tSystem.registerAudioProperty(p1); // INVALID CHANNEL
	//p1.channelID = 0;
	//p1.stride = 1;
	//tSystem.registerAudioProperty(p1); // VALID: STRIDE 1
	//p1.stride = 0;
	//p1.offset = 128;
	//tSystem.registerAudioProperty(p1); // VALID: OFFSET 128
	//p1.bufsize = 0;
	//tSystem.registerAudioProperty(p1); // INVALID: BUFSIZE 0

	Timestep ts{ Timepoint{},Timepoint{},Timepoint{} };
	tSystem.audioPropertyValues(ts);


	//std::shared_ptr<std::ifstream> ifs{ new std::ifstream{"200426_0165.wav",std::ios::binary} };
	//AudioReader reader{ ifs }; // samples: 13263836

	//reader.open();
	//{
	//	std::cout << "\n\tCase 2: Within cache" << std::endl;
	//	std::vector<float> test{ reader.audio(0u, 128u) };
	//	for (size_t i = 0; i < test.size(); ++i)
	//		std::cout << test[i] << ",";
	//}
	//{
	//	std::cout << "\n\tCase 3: Within file, out of cache" << std::endl;
	//	std::vector<float> test{ reader.audio(1024u, 128u) };
	//	for (size_t i = 0; i < test.size(); ++i)
	//		std::cout << test[i] << ",";
	//}
	//{
	//	std::cout << "\n\tCase 2: Within file, within new cache" << std::endl;
	//	std::vector<float> test{ reader.audio(1200u, 200u) };
	//	for (size_t i = 0; i < test.size(); ++i)
	//		std::cout << test[i] << ",";
	//}
	//{
	//	std::cout << "\n\tCase 4: Within cache, causing extension" << std::endl;
	//	std::vector<float> test{ reader.audio(1300u, 200u) };
	//	for (size_t i = 0; i < test.size(); ++i)
	//		std::cout << test[i] << ",";
	//}
	//{
	//	std::cout << "\n\tCase 2: Within file, within extended cache" << std::endl;
	//	std::vector<float> test{ reader.audio(2400u, 200u) };
	//	for (size_t i = 0; i < test.size(); ++i)
	//		std::cout << test[i] << ",";
	//}
	//{
	//	std::cout << "\n\tCase 1A: starts out of file bounds" << std::endl;
	//	std::vector<float> test{ reader.audio(13263836u, 128u) };
	//	for (size_t i = 0; i < test.size(); ++i)
	//		std::cout << test[i] << ",";
	//}
	//{
	//	std::cout << "\n\tCase 1B: starts within, ends out of file bounds" << std::endl;
	//	std::vector<float> test{ reader.audio(3315939u, 128u) };
	//	for (size_t i = 0; i < test.size(); ++i)
	//		std::cout << test[i] << ",";
	//}
	//reader.close();


	return 0;
}