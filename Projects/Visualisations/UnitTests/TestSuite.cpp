#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <Visualisations/Buffer.h>
#include <Visualisations/Recorder.h>
#include <portaudio\portaudio.h>
#include <Visualisations/FFMPEG_Encoder.h>

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

		/* This is unreliable: The unit test can't have expectations over the contents
								of the audio buffer.*/
		//REQUIRE(peak1 > 0.0f);
		//REQUIRE(peak2 > 0.0f);
	}
	SECTION("Audio Analysis: Minima")
	{
		float min1 = buf1.amplitude_minimum();
		float min2 = buf2.amplitude_minimum();
		INFO("First Buffer Minimum is: " << min1);
		INFO("Second Buffer Minimum is: " << min2);
		/* Ditto ^ */
		//REQUIRE(min1 < 0.0f);
		//REQUIRE(min2 < 0.0f);
	}
}

TEST_CASE("FFMPEG Encoder")
{
	FFMPEG_Encoder encoder{};

	/* We need a context for opengl to do anything. */
	auto setupContext = []() {
		if (!glfwInit())
			return (GLFWwindow*)0;
		// GL 3.0 + GLSL 130
		std::string glsl_version{ "#version 330" };
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); /* 4.5 is our latest version, reduce this for public release. */
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
		glfwWindowHint(GLFW_SAMPLES, 4); /* MSAA */

		// Create window with graphics context
		GLFWwindow* window = glfwCreateWindow(1920, 1080, "Unit Test Window Context", NULL, NULL);
		if (window == NULL)
			return (GLFWwindow*)0;
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // Enable vsync (Note: Disable if you want to test performance)
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			glfwTerminate();
			return (GLFWwindow*)0;
		}
		glEnable(GL_MULTISAMPLE); /* MSAA */
		return window;
	};

	GLFWwindow * win = setupContext();

	SECTION("FFMPEG: {codec: MPEG1, res: 1280*720, fps: 30fps, 300frames, out: unitTestFile.mpg}")
	{

		FFMPEG_Encoder::StartResult startResult = encoder.ffmpeg_encoder_start("unitTestFile.mpg", AVCodecID::AV_CODEC_ID_MPEG1VIDEO, 30, 1280, 720);
		REQUIRE(startResult == FFMPEG_Encoder::StartResult::Success);
		for (int i = 0; i < 300; ++i)
		{
			glClearColor(sin(glfwGetTime()), sin(0.2 * glfwGetTime()), cos(0.3 * glfwGetTime()), 1.0);
			glClear(GL_COLOR_BUFFER_BIT);
			encoder.ffmpeg_encoder_render_frame();
		}
		FFMPEG_Encoder::FinishResult endResult = encoder.ffmpeg_encoder_finish();
		REQUIRE(endResult == FFMPEG_Encoder::FinishResult::Success);
	}
}