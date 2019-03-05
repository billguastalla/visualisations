#include <iostream>

#include "AudioReader.h"

int main()
{
	AudioReader reader{};

	reader.addFile("181021_0038.wav");
	reader.addFile("INVALID.wav");

	return 0;
}