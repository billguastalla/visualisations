#pragma once
#include <string>

enum class DurationMode
{
	Frames, /* */
	Samples,
	Time
};

class Settings_VideoRendering
{
public:
	/*
		-> Settings should be responsible for validating whether it is:
			(a) self-consistent
				(don't have an example here yet but might be necessary)
			(b) consistent with other settings
				(for instance making sure that:
					-> audio channels we are recording actually exist
					-> resolution matches format
				)
	*/
	Settings_VideoRendering();
	~Settings_VideoRendering();

private:
	/*
		-> Encoder needs:
			-> Filename
			-> FPS
			-> Width
			-> Height
	*/


	std::string m_outputFileName;

	DurationMode m_renderDurationMode;
	int m_renderDuration;

	bool m_renderUserInterface;

	bool m_recordAudio;
	/* TODO: Add audio settings. There's a lot that needs to be done before this happens though
				and this explains it:
		-> Decide various things:
			-> Is channel count important or is it always two? (Btw check if 5.1 is supported on youtube)
			-> If you are having multiple audio inputs, which you plan to do:
				In what part of the program are you combining channels into the output audio file?
				-> A: Probably best to do this in the renderer itself, 
					-> You might have some audio combiner class which can create a stream of combined audio.
					-> The settings could just hold the indexes of input channels, mapped to output channels.
						-> would have to be a multimap or some channel mapping object to allow flexible range
							& domain.
					-> The audio combiner probably needs to be its own  thing because:
						-> It has other uses than just preparing audio frames for output: for instance
							you can use it to create new inputs for the visualisation.
							-> Given this, the audio 
	*/
	
	int m_codecOption;
	int m_fileFormatOption;
};
