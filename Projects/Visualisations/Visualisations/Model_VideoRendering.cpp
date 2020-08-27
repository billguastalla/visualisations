#include "Program.h"
#include "PostProcessing.h"

#include "Model_VideoRendering.h"

#include "Settings_VideoRendering.h"
#include "FFMPEG_Encoder.h"
#include "Timestring.h"

#include <GLFW/glfw3.h>
#include <boost/property_tree/ptree.hpp>
#include <stbimage/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbimage/stb_image_write.h>


Model_VideoRendering::Model_VideoRendering(Program * program, GLFWwindow* window)
	:
	m_encoder{ new FFMPEG_Encoder{} },
	m_recordState{ RecordState::Stopped },
	m_frameRate{ 60 },
	m_frameCount{ 0 },
	m_fileName{},
	m_renderUI{ true },
	m_recordAudio{ false },

	m_window{ window },
	p_program{program}
{
	resetFilename();
}

Model_VideoRendering::~Model_VideoRendering()
{
}

void Model_VideoRendering::renderFrame()
{
	if (m_recordState == RecordState::Started)
	{
		GLuint originalFramebuffer{ p_program->postProcessing()->currentFramebuffer() };
		p_program->postProcessing()->setCurrentFramebuffer(p_program->postProcessing()->mainFramebuffer());
		std::array<int, 2> mainRes{ p_program->postProcessing()->mainFramebufferResolution() };

		m_encoder->ffmpeg_encoder_render_frame();

		p_program->postProcessing()->setCurrentFramebuffer(originalFramebuffer);


		m_frameCount = m_encoder->currentFrame();
	}
}

void Model_VideoRendering::takePicture(const std::string& filename)
{
	GLuint originalFramebuffer{ p_program->postProcessing()->currentFramebuffer() };
	p_program->postProcessing()->setCurrentFramebuffer(p_program->postProcessing()->mainFramebuffer());
	std::array<int, 2> mainRes{ p_program->postProcessing()->mainFramebufferResolution() };

	std::vector<GLubyte> pixelData{};
	pixelData.resize(4u * (size_t)(mainRes[0] * (size_t)mainRes[1]) );
	glReadPixels(0, 0, mainRes[0], mainRes[1], GL_RGBA, GL_UNSIGNED_BYTE, &pixelData[0]);
	stbi_write_png(filename.c_str(), mainRes[0], mainRes[1], 4, &pixelData[0],0);

	p_program->postProcessing()->setCurrentFramebuffer(originalFramebuffer);
}

bool Model_VideoRendering::start()
{
	if (m_recordState == RecordState::Stopped)
	{
		m_frameCount = 0;
		//int width{ 1920 }, height{ 1080 };
		//glfwGetWindowSize(m_window, &width, &height);
		std::array<int, 2> mainResolution{ p_program->postProcessing()->mainFramebufferResolution() };
		FFMPEG_Encoder::StartResult res = m_encoder->ffmpeg_encoder_start(m_fileName.c_str(), AVCodecID::AV_CODEC_ID_MPEG1VIDEO, m_frameRate, mainResolution[0], mainResolution[1]);
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

void Model_VideoRendering::resetFilename()
{
	m_fileName = std::string{ "video-" + getTimeStr() + ".mpg" };
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


