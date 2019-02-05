#pragma once
#include <memory>

class Settings_VideoRendering;
class FFMPEG_Encoder;

class Controller_VideoRendering
{
public:
	/*
		-> Some things the controller needs to do:
			-> Start/stop recording and keep track of this:
				-> Prevent various settings being modified while recording
			->
			-> Handle a separate framebuffer, and handle *what it is that the framebuffer draws*.
				-> (UI enabled/disabled option)
			-> Handle the width/height of the framebuffer, and permit/forbid modification of
				width & height during recording
	*/
	Controller_VideoRendering(std::shared_ptr<Settings_VideoRendering> & settings);
	~Controller_VideoRendering();

private:
	std::shared_ptr<Settings_VideoRendering> m_settings;
	std::shared_ptr<FFMPEG_Encoder> m_encoder;
};
