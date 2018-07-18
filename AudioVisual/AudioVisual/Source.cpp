#include "Recorder.h"
#include <iostream>

int main()
{


	Recorder test{};
	test.startMonitoring();


	std::vector<Buffer> buffers;
	for (int i = 0; i < 100; ++i)
	{
		Pa_Sleep(100);
		Buffer buf = test.getBuffer();
		buffers.push_back(buf);
	}
	for (int i = 0; i < (int)buffers.size(); ++i)
	{
		std::cout << "Buffer " << i << ":\n";
		std::cout << "\tSample Count: " << buffers[i].sampleCount() << "\n";
		std::cout << "\tMin: " << buffers[i].amplitude_minimum() << "\n";
		std::cout << "\tMax: " << buffers[i].amplitude_peak() << "\n";
		std::cout << "\tAverage: " << buffers[i].amplitude_average() << "\n";
	}



	test.stopMonitoring();
}