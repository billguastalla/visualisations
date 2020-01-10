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

#pragma warning(disable 4996)
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



struct MuxerSettings
{
	std::string fileName;
	double m_streamDuration = 10.0;
	int m_framerate = 60; /* 25 images/s */
	AVPixelFormat m_pixelFormat = AV_PIX_FMT_YUV420P; /* default pix_fmt */
};


// a wrapper around a single output AVStream
struct OutputStream;

class FFMPEG_Muxer
{
public:
	static void log_packet(const AVFormatContext* fmt_ctx, const AVPacket* pkt);

	static int write_frame(AVFormatContext* fmt_ctx, const AVRational* time_base, AVStream* st, AVPacket* pkt);

	/* Add an output stream. */
	static void add_stream(OutputStream* ost, AVFormatContext* oc, AVCodec** codec, enum AVCodecID codec_id);

	/**************************************************************/
	/* audio output */
	static AVFrame* alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples);

	static void open_audio(AVFormatContext* oc, AVCodec* codec, OutputStream* ost, AVDictionary* opt_arg);

	/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
	 * 'nb_channels' channels. */
	static AVFrame* get_audio_frame(OutputStream* ost);

	/*
	 * encode one audio frame and send it to the muxer
	 * return 1 when encoding is finished, 0 otherwise
	 */
	static int write_audio_frame(AVFormatContext* oc, OutputStream* ost);

	static AVFrame* alloc_picture(enum AVPixelFormat pix_fmt, int width, int height);

	static void open_video(AVFormatContext* oc, AVCodec* codec, OutputStream* ost, AVDictionary* opt_arg);

	static void fill_yuv_image(AVFrame* pict, int frame_index, int width, int height);

	static AVFrame* get_video_frame(OutputStream* ost);


	/*
	 * encode one video frame and send it to the muxer
	 * return 1 when encoding is finished, 0 otherwise
	 */
	static int write_video_frame(AVFormatContext* oc, OutputStream* ost);

	static void close_stream(AVFormatContext* oc, OutputStream* ost);
};

/**************************************************************/
/* media file output */
//int main(int argc, char** argv)
//{
//	OutputStream video_st = { 0 }, audio_st = { 0 };
//	const char* filename;
//	AVOutputFormat* fmt;
//	AVFormatContext* oc;
//	AVCodec* audio_codec, * video_codec;
//	int ret;
//	int have_video = 0, have_audio = 0;
//	int encode_video = 0, encode_audio = 0;
//	AVDictionary* opt = NULL;
//	int i;
//
//	if (argc < 2) {
//		printf("usage: %s output_file\n"
//			"API example program to output a media file with libavformat.\n"
//			"This program generates a synthetic audio and video stream, encodes and\n"
//			"muxes them into a file named output_file.\n"
//			"The output format is automatically guessed according to the file extension.\n"
//			"Raw images can also be output by using '%%d' in the filename.\n"
//			"\n", argv[0]);
//		return 1;
//	}
//
//	filename = argv[1];
//	for (i = 2; i + 1 < argc; i += 2) {
//		if (!strcmp(argv[i], "-flags") || !strcmp(argv[i], "-fflags"))
//			av_dict_set(&opt, argv[i] + 1, argv[i + 1], 0);
//	}
//
//	/* allocate the output media context */
//	avformat_alloc_output_context2(&oc, NULL, NULL, filename);
//	if (!oc) {
//		printf("Could not deduce output format from file extension: using MPEG.\n");
//		avformat_alloc_output_context2(&oc, NULL, "mpeg", filename);
//	}
//	if (!oc)
//		return 1;
//
//	fmt = oc->oformat;
//
//	/* Add the audio and video streams using the default format codecs
//	 * and initialize the codecs. */
//	if (fmt->video_codec != AV_CODEC_ID_NONE) {
//		add_stream(&video_st, oc, &video_codec, fmt->video_codec);
//		have_video = 1;
//		encode_video = 1;
//	}
//	if (fmt->audio_codec != AV_CODEC_ID_NONE) {
//		add_stream(&audio_st, oc, &audio_codec, fmt->audio_codec);
//		have_audio = 1;
//		encode_audio = 1;
//	}
//
//	/* Now that all the parameters are set, we can open the audio and
//	 * video codecs and allocate the necessary encode buffers. */
//	if (have_video)
//		open_video(oc, video_codec, &video_st, opt);
//
//	if (have_audio)
//		open_audio(oc, audio_codec, &audio_st, opt);
//
//	av_dump_format(oc, 0, filename, 1);
//
//	/* open the output file, if needed */
//	if (!(fmt->flags & AVFMT_NOFILE)) {
//		ret = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE);
//		if (ret < 0) {
//			fprintf(stderr, "Could not open '%s': %s\n", filename,
//				av_err2str(ret));
//			return 1;
//		}
//	}
//
//	/* Write the stream header, if any. */
//	ret = avformat_write_header(oc, &opt);
//	if (ret < 0) {
//		fprintf(stderr, "Error occurred when opening output file: %s\n",
//			av_err2str(ret));
//		return 1;
//	}
//
//	while (encode_video || encode_audio) {
//		/* select the stream to encode */
//		if (encode_video &&
//			(!encode_audio || av_compare_ts(video_st.next_pts, video_st.enc->time_base,
//				audio_st.next_pts, audio_st.enc->time_base) <= 0)) {
//			encode_video = !write_video_frame(oc, &video_st);
//		}
//		else {
//			encode_audio = !write_audio_frame(oc, &audio_st);
//		}
//	}
//
//	/* Write the trailer, if any. The trailer must be written before you
//	 * close the CodecContexts open when you wrote the header; otherwise
//	 * av_write_trailer() may try to use memory that was freed on
//	 * av_codec_close(). */
//	av_write_trailer(oc);
//
//	/* Close each codec. */
//	if (have_video)
//		close_stream(oc, &video_st);
//	if (have_audio)
//		close_stream(oc, &audio_st);
//
//	if (!(fmt->flags & AVFMT_NOFILE))
//		/* Close the output file. */
//		avio_closep(&oc->pb);
//
//	/* free the stream */
//	avformat_free_context(oc);
//
//	return 0;
//}
