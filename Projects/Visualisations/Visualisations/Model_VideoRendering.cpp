#include "Model_VideoRendering.h"

#include "Settings_VideoRendering.h"
#include "FFMPEG_Encoder.h"

#include <GLFW/glfw3.h>

Model_VideoRendering::Model_VideoRendering(std::shared_ptr<Settings_VideoRendering>& settings, GLFWwindow* window)
	:
	m_settings{ settings },
	m_encoder{ new FFMPEG_Encoder{} },
	m_recordState{ RecordState::Stopped },
	m_frameRate{ 30 },
	m_frameCount{ 0 },
	m_fileName{ "VideoRenderModule.mpg" },
	m_renderUI{ true },
	m_recordAudio{ false },

	m_window{ window }
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
		m_frameCount = m_encoder->currentFrame();
	}
}

bool Model_VideoRendering::start()
{
	if (m_recordState == RecordState::Stopped)
	{
		m_frameCount = 0;
		int width{ 1920 }, height{ 1080 };
		glfwGetWindowSize(m_window, &width, &height);
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
	return false;
}

bool Model_VideoRendering::setFrameRate(int fr)
{
	if (m_recordState != RecordState::Started)
	{
		m_frameRate = fr;
		return true;
	}
	return false;
}

const std::string Model_VideoRendering::framerateOptionsString() const
{
	std::string result{};
	for (int i = 15; i < 240; i *= 2)
	{
		std::string fps{ std::to_string(i) };
		fps += '\0';
		result += fps;
	}
	return result;
}
const int Model_VideoRendering::combo_FRtoOPT(int fr) const
{
	std::vector<int> result{};
	int idx = 0;
	for (int i = 15; i < 240; i *= 2)
	{
		if (fr == i)
			return idx;
		++idx;
	}
	return -1;
}
const int Model_VideoRendering::combo_OPTToFR(int opt) const
{
	std::vector<int> result{};
	for (int i = 15; i < 240; i *= 2)
		result.push_back(i);
	if (opt < 0 || opt >= result.size())
		return 30;
	else
		return result[opt];
}


