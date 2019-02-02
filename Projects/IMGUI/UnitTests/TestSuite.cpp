#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <IMGUI\Buffer.h>
#include <IMGUI\Recorder.h>
#include <portaudio\portaudio.h>


TEST_CASE("PORTAUDIO: Devices Exist")
{
	Pa_Initialize();
	SECTION("PortAudio Initialised correctly")
	{
		Pa_Initialize();
	}


	PaDeviceIndex deviceCount = Pa_GetDeviceCount();
	PaHostApiIndex hostApiCount = Pa_GetHostApiCount();
	SECTION("Devices & APIs Exist")
	{
		REQUIRE(hostApiCount != 0);
		REQUIRE(deviceCount != 0);
	}

	PaDeviceIndex defaultInputIdx = Pa_GetDefaultInputDevice();
	PaDeviceIndex defaultOutputIdx = Pa_GetDefaultOutputDevice();
	PaHostApiIndex defaultHostApiIdx = Pa_GetDefaultHostApi();
	SECTION("Default Devices & Hosts exist")
	{
		REQUIRE(defaultInputIdx != -1);
		REQUIRE(defaultOutputIdx != -1);
		REQUIRE(defaultHostApiIdx != -1);
	}

	std::vector<const PaDeviceInfo*> devices{};
	for (int i = 0; i < deviceCount; ++i)
		devices.push_back(Pa_GetDeviceInfo(i));
	std::vector<const PaHostApiInfo*> hostApis{};
	for (int i = 0; i < hostApiCount; ++i)
		hostApis.push_back(Pa_GetHostApiInfo(i));

	SECTION("Hosts & Devices are retrievable")
	{
		for (int i = 0; i < deviceCount; ++i)
			REQUIRE(devices[i] != nullptr);
		for (int i = 0; i < hostApiCount; ++i)
			REQUIRE(hostApis[i] != nullptr);
	}


	Pa_Terminate();
}

TEST_CASE("Check Buffers")
{
	Recorder rec{};
	rec.startMonitoring();
	Sleep(1000);
	Buffer buf1 = rec.getBuffer();

	Recorder rec2{};
	rec2.startMonitoring();
	Sleep(1000);
	Buffer buf2 = rec.getBuffer();

	rec.stopMonitoring();
	rec2.stopMonitoring();

	SECTION("Audio Analysis: Peaks")
	{
		float peak1 = buf1.amplitude_peak();
		float peak2 = buf2.amplitude_peak();
		INFO("First Buffer Peak is: " << peak1);
		INFO("Second Buffer Peak is: " << peak2);
		REQUIRE(peak1 > 0.0f);
		REQUIRE(peak2 > 0.0f);
	}
	SECTION("Audio Analysis: Minima")
	{
		float min1 = buf1.amplitude_minimum();
		float min2 = buf2.amplitude_minimum();
		INFO("First Buffer Minimum is: " << min1);
		INFO("Second Buffer Minimum is: " << min2);
		REQUIRE(min1 < 0.0f);
		REQUIRE(min2 < 0.0f);
	}
}
