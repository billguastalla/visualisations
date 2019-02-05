#include "Controller_VideoRendering.h"

#include "Settings_VideoRendering.h"
#include "FFMPEG_Encoder.h"

Controller_VideoRendering::Controller_VideoRendering(std::shared_ptr<Settings_VideoRendering> & settings)
	: m_settings{ settings }, m_encoder{ new FFMPEG_Encoder{} }
{
}

Controller_VideoRendering::~Controller_VideoRendering()
{
}

