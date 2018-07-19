#include "Recorder.h"
#include "Renderer.h"
#include <iostream>

int main()
{


	Recorder test{};
	test.startMonitoring();

	Renderer render{};

	render.renderLoop(test);

	test.stopMonitoring();
}

