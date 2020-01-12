#include "FFMPEG_Stream.h"

FFMPEG_Stream::FFMPEG_Stream()
	:
	m_avstream{ nullptr },
	m_avcodecEncoderContext{ nullptr },
	m_nextFramePTS{ 0 },
	samples_count{ 0 },
	m_streamFrame{ nullptr },
	m_streamFrameTemp{ nullptr },
	p_codec{ nullptr },
	m_initialised{ false },
	m_finished{ false }
{
}

void FFMPEG_Stream::deallocate()
{
	//if (m_allocation & StreamAllocation::CodecContext)
	//	avcodec_free_context(&m_avcodecEncoderContext);
	//if (m_allocation & StreamAllocation::Frame_Main)
	//	av_frame_free(&frame);
	//if (m_allocation & StreamAllocation::Frame_Temp)
	//	av_frame_free(&tmp_frame);
	//if (m_allocation & StreamAllocation::SWS_Context)
	//	sws_freeContext(sws_ctx);
	//if (m_allocation & StreamAllocation::SWR_Context)
	//	swr_free(&swr_ctx);
	//m_allocation = StreamAllocation::None;
}
