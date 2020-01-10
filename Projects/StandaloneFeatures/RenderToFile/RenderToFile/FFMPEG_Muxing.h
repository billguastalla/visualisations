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

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>

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

struct MuxerSettings /* Building the needed configuration for muxing. */
{
	std::string m_fileName;
	double m_streamDuration = 10.0;

	/* Video */
	int m_framerate = 60; /* 25 images/s */
	AVPixelFormat m_pixelFormat = AV_PIX_FMT_YUV420P; /* default pix_fmt */
	int m_frameWidth = 1920;
	int m_frameHeight = 1080;
	int m_videoBitRate = 400000;

	/* Audio */
	int m_audioBitRate = 64000;
	int m_audioDefaultSampleRate = 44100;
	uint64_t m_audioChannels = AV_CH_LAYOUT_STEREO;
};

// a wrapper around a single output AVStream
typedef struct OutputStream {
	AVStream* st;
	AVCodecContext* enc;

	/* pts of the next frame that will be generated */
	int64_t next_pts;
	int samples_count;

	AVFrame* frame;
	AVFrame* tmp_frame;

	float t, tincr, tincr2;

	struct SwsContext* sws_ctx;
	struct SwrContext* swr_ctx;
} OutputStream;

class FFMPEG_Muxer
{
public:
	/* true on success */
	bool initialise(MuxerSettings settings);
	void run();
	void deinitialise();

private:
	void log_packet(const AVFormatContext* fmt_ctx, const AVPacket* pkt);

	int write_frame(AVFormatContext* fmt_ctx, const AVRational* time_base, AVStream* st, AVPacket* pkt);

	/* Add an output stream. */
	void add_stream(OutputStream* ost, AVFormatContext* oc, AVCodec** codec, enum AVCodecID codec_id);

	/**************************************************************/
	/* audio output */
	AVFrame* alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples);

	void open_audio(AVFormatContext* oc, AVCodec* codec, OutputStream* ost, AVDictionary* opt_arg);

	/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
	 * 'nb_channels' channels. */
	AVFrame* get_audio_frame(OutputStream* ost);

	/*
	 * encode one audio frame and send it to the muxer
	 * return 1 when encoding is finished, 0 otherwise
	 */
	int write_audio_frame(AVFormatContext* oc, OutputStream* ost);

	AVFrame* alloc_picture(enum AVPixelFormat pix_fmt, int width, int height);

	void open_video(AVFormatContext* oc, AVCodec* codec, OutputStream* ost, AVDictionary* opt_arg);

	void fill_yuv_image(AVFrame* pict, int frame_index, int width, int height);

	AVFrame* get_video_frame(OutputStream* ost);

	/*
	 * encode one video frame and send it to the muxer
	 * return 1 when encoding is finished, 0 otherwise
	 */
	int write_video_frame(AVFormatContext* oc, OutputStream* ost);

	void close_stream(AVFormatContext* oc, OutputStream* ost);

private:
	MuxerSettings m_settings;

	OutputStream video_st = { 0 }, audio_st = { 0 };
	const char* filename{ nullptr };
	AVOutputFormat* fmt{ nullptr }; /* Owned by the AVFormatContext, oc. Holds output format information. */
	AVFormatContext* oc{ nullptr }; /* <- B.G Takes the filename, allocated with avformat_alloc_output_context2() */
	AVCodec* audio_codec{ nullptr }, * video_codec{ nullptr };
	int ret{ 0 };
	int have_video{ 0 }, have_audio{ 0 };
	int encode_video{ 0 }, encode_audio{ 0 };
	AVDictionary* opt{ nullptr };
};