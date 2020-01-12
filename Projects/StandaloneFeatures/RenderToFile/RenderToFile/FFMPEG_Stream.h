#pragma once
#define __STDC_CONSTANT_MACROS
extern "C"
{
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
#include <vector>
#include <string>
#include <algorithm>

class FFMPEG_Stream
{
protected:
	/* B.G TODO: Deal with errors.*/
	void reportError(std::string error, int num = 0) {};
public:
	/* B.G: TODO: No default-constructed OutputStreams, only constructors with a context, and some MuxerAudio/MuxerVideo
					settings.
					-> Then we get the ability to create a safe interface.
	*/
	FFMPEG_Stream();

	/* B.G Replacement for get_audio_frame() and get_video_frame() */
	virtual AVFrame * getFrame() = 0;

	/* B.G Replacement for alloc_audio_frame() and alloc_picture(). Not used: different parameters between audio and video. */
	//virtual AVFrame* allocateFrame() = 0;

	/* B.G Replacement for write_audio_frame() and write_video_frame(). */
	/* B.G Pass an uninitialised packet, returns 0 on success. */
	virtual bool buildPacket(AVPacket& packet, AVFormatContext* oc) = 0;

	/* B.G Replacement for open_audio() and open_video() */
	virtual void open(AVFormatContext* oc, AVDictionary* opt_arg) = 0;

	/* B.G Replacement for close_stream() */
	virtual void close() = 0;

	/* B.G: Compare the timecodes of each stream to find which stream to encode the next frame from. */
	static FFMPEG_Stream * nextStream(std::vector<FFMPEG_Stream*> & streams)
	{
		auto i = std::max_element(streams.begin(), streams.end(),
			[](const FFMPEG_Stream* one, const FFMPEG_Stream* two)
			{
				int comp = (av_compare_ts(one->m_nextFramePTS, one->m_avcodecEncoderContext->time_base, two->m_nextFramePTS, two->m_avcodecEncoderContext->time_base));
				return (comp > 0);
			});
		return *i;
	}
	/* B.G : TODO: Who is responsible for the time at which streams are finished?
				I suspect that all frames of each stream need to be written up to a single end-time, to make videos look smooth.
				Leave it this way for now.
	*/
	static bool streamsUnfinished(std::vector<FFMPEG_Stream*> & streams, double endTime)
	{
		for (auto i = streams.begin(); i != streams.end(); ++i)
			if (av_compare_ts((*i)->m_nextFramePTS, (*i)->m_avcodecEncoderContext->time_base, (int64_t)endTime, av_make_q(1, 1)) < 0)
				return true;
		return false;
	}

	AVStream * m_avstream; /* ALLOCATION: Allocated independently, but owned by an AVFormatContext (I think)
								Not explicitly de-allocated. */
	AVCodecContext* m_avcodecEncoderContext;/* ALLOCATION: Allocated independently  */


	/* pts of the next frame that will be generated */
	int64_t m_nextFramePTS;
	int samples_count;

	AVFrame* m_streamFrame; /* ALLOCATION: Allocated independently  */
	AVFrame* m_streamFrameTemp; /* ALLOCATION: Allocated independently  */

	/* Bill: New fields*/
	AVCodec* p_codec = nullptr;
	bool m_initialised = false; // B.G: Replacement for have_audio/have_video
	bool m_finished = false; // B.G: (inverted) replacement for encode_audio/encode_video

	void deallocate();
};








//enum StreamAllocation
//{
//	None = 0x00,
//	//Stream = 0x01,
//	CodecContext = 0x01,
//	Frame_Main = 0x02,
//	Frame_Temp = 0x04,
//	SWS_Context = 0x08,
//	SWR_Context = 0x10,
//} m_allocation;
