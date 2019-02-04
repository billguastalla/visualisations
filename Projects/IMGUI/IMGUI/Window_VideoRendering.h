#pragma once
#include <memory>

class FFMPEG_Encoder;

class Window_VideoRendering
{
public:
	Window_VideoRendering(std::shared_ptr<FFMPEG_Encoder> & encoder);
	~Window_VideoRendering();

	void draw();
private:
	std::shared_ptr<FFMPEG_Encoder> m_encoder;
};

