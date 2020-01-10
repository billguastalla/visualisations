#include "FFMPEG_EncoderCMD.h"

/* Dates & Times for output file */
#include <ctime>
#include <sstream>


FFMPEG_EncoderCMD::FFMPEG_EncoderCMD(unsigned int width, unsigned int height)
	:
	m_width{ width },
	m_height{ height },
	m_buffer{ nullptr },
	m_ffmpeg{ nullptr },
	m_started{ false }
{
}

void FFMPEG_EncoderCMD::start()
{
	if (!m_started)
	{
		/* Get time to make unique mp4s, and we don't mind about the format of the string. */
		auto t = std::time(nullptr);
		std::ostringstream oss;
		oss << t;

		// start ffmpeg telling it to expect raw rgba 720p-60hz frames
		// -i - tells it to read frames from stdin
		std::string cmd{ "ffmpeg -r 60 -f rawvideo -pix_fmt rgba -s 1600x900 -i - "
			"-threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip " };
		cmd.append("output");
		cmd.append(oss.str());
		cmd.append(".mp4");
		// open pipe to ffmpeg's stdin in binary write mode
		m_ffmpeg = _popen(cmd.c_str(), "wb");
		/* Set up buffer */
		m_buffer = new int[(size_t)m_width * (size_t)m_height];
	}
	m_started = true;
}

void FFMPEG_EncoderCMD::renderFrame()
{
	if (m_started)
	{
		/* ********************* FFMPEG ***************** */
		/* Read pixels from GPU into buffer */
		glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
		/* Write buffer into FFMPEG */
		fwrite(m_buffer, sizeof(int)*m_width*m_height, 1, m_ffmpeg);
		/* ********************************************** */
	}
}

void FFMPEG_EncoderCMD::stop()
{
	/* ********************* FFMPEG ***************** */
	_pclose(m_ffmpeg);/* Bill: Check if this actually deletes m_ffmpeg. Also you should convert this to ofstream. */
	m_ffmpeg = nullptr;
	delete[] m_buffer;
	m_buffer = nullptr;
	m_started = false;
	/* ********************************************** */
}
