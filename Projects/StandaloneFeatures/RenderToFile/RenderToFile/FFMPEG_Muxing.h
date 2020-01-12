#pragma once
/*
 * Copyright (c) 2003 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
//#pragma warning(disable 4996)
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

#include "FFMPEG_Stream.h"

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

struct MuxerSettings /* Building the needed configuration for muxing. */
{
	std::string m_fileName = "default.mp4";
	double m_streamDuration = 10.5;

	/* Video */
	int m_framerate = 60; /* 25 images/s */
	AVPixelFormat m_pixelFormat = AV_PIX_FMT_YUV420P; /* default pix_fmt is AV_PIX_FMT_YUV420P*/
	int m_frameWidth = 640;
	int m_frameHeight = 480;
	int m_videoBitRate = 40000000;

	/* Audio */
	int m_audioBitRate = 320000;
	int m_audioDefaultSampleRate = 48000;
	uint64_t m_audioChannels = AV_CH_LAYOUT_STEREO;
};

class FFMPEG_Muxer
{
public:
	FFMPEG_Muxer()
		:
		m_initialised{ false },
		m_allocationStage{ AllocationStage::None }
	{
	}
	bool initialise(MuxerSettings settings);
	void run();
	void deinitialise();

private:
	void reportError(std::string description, int errorNum = 0);
	void log_packet(const AVFormatContext* fmt_ctx, const AVPacket* pkt);


	int write_frame(AVFormatContext* fmt_ctx, const AVRational* time_base, AVStream* m_avstream, AVPacket* pkt);
	/* Codec** will be set up by this function. */
	bool add_stream(FFMPEG_Stream * ost, AVFormatContext* oc, AVCodec** codec, enum AVCodecID codec_id);

private:
	MuxerSettings m_settings;
	bool m_initialised;

	const char* filename{ nullptr };

	/* B.G:
		-> This is the Output Media Context
		-> Takes the filename, allocated with avformat_alloc_output_context2().
	*/
	AVFormatContext* oc{ nullptr };

	/* Owned by the AVFormatContext, oc. Holds output format information. */
	AVOutputFormat* fmt{ nullptr };

	AVCodec * audio_codec{ nullptr },* video_codec{ nullptr };
	int ret{ 0 };
	AVDictionary* opt{ nullptr };

	/* B.G: Vector of streams. */
	std::vector<FFMPEG_Stream*> m_streams;

	/* Keep track of what was allocated, so we can free what we need to,
		to minimise fatal exits. */
	enum AllocationStage
	{
		None = 0x00,
		Output_FormatContext = 0x01,
		OutputStream_Stream = 0x02,
		OutputStream_CodecContext = 0x04,

		OutputStream_AudioFrame = 0x080,

	} m_allocationStage;
	bool deallocate();
};