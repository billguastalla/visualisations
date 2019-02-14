#include "Model_VideoRendering.h"

#include "Settings_VideoRendering.h"
#include "FFMPEG_Encoder.h"

Model_VideoRendering::Model_VideoRendering(std::shared_ptr<Settings_VideoRendering> & settings)
	: m_settings{ settings }, m_encoder{ new FFMPEG_Encoder{} }, m_recordState{ RecordState::Stopped }, m_frameRate{ 30 }, m_fileName{ "VideoRenderModule.mpg" }
{

}

Model_VideoRendering::~Model_VideoRendering()
{
}

void Model_VideoRendering::renderFrame()
{
	if (m_recordState == RecordState::Started)
	{
		m_encoder->ffmpeg_encoder_render_frame();
	}
}

bool Model_VideoRendering::start()
{
	if (m_recordState == RecordState::Stopped)
	{
		int width{ 1920 }, height{ 1080 };
		FFMPEG_Encoder::StartResult res = m_encoder->ffmpeg_encoder_start(m_fileName.c_str(), AVCodecID::AV_CODEC_ID_MPEG1VIDEO, m_frameRate, width, height);
		if (res == FFMPEG_Encoder::StartResult::Success)
		{
			m_recordState = RecordState::Started;
			return true;
		}
	}
	return false;
}

bool Model_VideoRendering::pause()
{
	if (m_recordState == RecordState::Started)
		m_recordState = RecordState::Paused;
	else if (m_recordState == RecordState::Paused)
		m_recordState = RecordState::Started;
	else
		return false;
	return true;
}

bool Model_VideoRendering::stop()
{
	if (m_recordState == RecordState::Started || m_recordState == RecordState::Paused)
	{
		FFMPEG_Encoder::FinishResult res = m_encoder->ffmpeg_encoder_finish();
		if (res == FFMPEG_Encoder::FinishResult::Success)
		{
			m_recordState = RecordState::Stopped;
			return true;
		}
	}
	else
		return false;
}
