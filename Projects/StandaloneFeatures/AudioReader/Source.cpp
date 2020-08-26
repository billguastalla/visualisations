#include "AudioReader.h"

#include <iostream>
#include <fstream>
int main()
{
	std::shared_ptr<std::ifstream> ifs{ new std::ifstream{"200426_0165.wav",std::ios::binary} };
	AudioReader reader{ ifs }; // samples: 13263836

	reader.open();
	{
		std::cout << "\n\tCase 2: Within cache" << std::endl;
		std::vector<float> test{ reader.audio(0u, 128u) };
		for (int i = 0; i < test.size(); ++i)
			std::cout << test[i] << ",";
	}
	{
		std::cout << "\n\tCase 3: Within file, out of cache" << std::endl;
		std::vector<float> test{ reader.audio(1024u, 128u) };
		for (int i = 0; i < test.size(); ++i)
			std::cout << test[i] << ",";
	}
	{
		std::cout << "\n\tCase 2: Within file, within new cache" << std::endl;
		std::vector<float> test{ reader.audio(1200u, 200u) };
		for (int i = 0; i < test.size(); ++i)
			std::cout << test[i] << ",";
	}
	{
		std::cout << "\n\tCase 4: Within cache, causing extension" << std::endl;
		std::vector<float> test{ reader.audio(1300u, 200u) };
		for (int i = 0; i < test.size(); ++i)
			std::cout << test[i] << ",";
	}
	{
		std::cout << "\n\tCase 2: Within file, within extended cache" << std::endl;
		std::vector<float> test{ reader.audio(2400u, 200u) };
		for (int i = 0; i < test.size(); ++i)
			std::cout << test[i] << ",";
	}
	{
		std::cout << "\n\tCase 1A: starts out of file bounds" << std::endl;
		std::vector<float> test{ reader.audio(13263836u, 128u) };
		for (int i = 0; i < test.size(); ++i)
			std::cout << test[i] << ",";
	}
	{
		std::cout << "\n\tCase 1B: starts within, ends out of file bounds" << std::endl;
		std::vector<float> test{ reader.audio(3315939u, 128u) };
		for (int i = 0; i < test.size(); ++i)
			std::cout << test[i] << ",";
	}
	reader.close();


	return 0;
}