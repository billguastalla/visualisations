#include <iostream>
#include "AudioInterface.h"
#include "AudioOutputMuxer.h"
#include "FFMPEG_AudioEncoder.h"

int main()
{
	/*
		-> Missing prototypes:
			-> Duration: Interface should expose a max duration in samples and time.
			(Should we use a specific object for the transport?)

		-> We want to:
			1a. Construct an AudioInterface.
				1b. Set the expected sample rate.
				1c. Load the audio files.
			2a. Construct an output muxer.
				2b. Create two output channels.
				2c. Map two input channels to two output channels.
			3a. Construct an audio encoder. (Create function that takes an output muxer)
				3b.
	*/
	/* 1a */
	AudioIO::AudioInterface inter{};
	/* 1b */
	/* 1c */
	//inter.addFile("SAMPLE_44100_32FPCM.wav");
	//inter.addFile("181030_0055.wav");
	/* 2a */

	inter.addSine("test",440,1.0);


	AudioIO::AudioOutputMuxer muxer{ &inter };

	FFMPEG_AudioEncoder encoder{};
	encoder.ffmpeg_encoder_start("testsine.mp3", 44100);
	
	int endSample = muxer.maximumSampleCount(), startSample = 0;
	while(startSample < endSample)
		startSample = encoder.ffmpeg_encoder_render_frame(muxer,startSample);

	encoder.ffmpeg_encoder_finish();
	return 0;
}