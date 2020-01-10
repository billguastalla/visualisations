/*
	TODO:
		- Provide an interface appropriate for the visualisation project.
		- Handle all exit(1) statements, and de-allocate appropriately,
		  with errors.
		- Replace all C-like fprint statements, initialise all variables,
		  erase global variables, make member variables and handle carefully.
		- Write tests for rendering sample video in all formats, in unittests.
		- Check with FFMPEG or StackOverflow/CodeReview that the code is appropriate.

		Streams:
			-> Allow external input of audio via interface.
			-> Allow external input of video via interface.

		- Publish. Reasons:
			-> Want to be able to run code and not exit program on failure.
			-> Want to be able to re-run muxer multiple times with different settings.
			-> Want to be able to see the structure clearly.
			-> Want to run this in the C++11 standard without warnings.
*/
/*
	Bill Guastalla 2019: MUXING:
		This is a quick port from C -> C++ for a code example found in the FFMPEG
		documentation. The original work is by Fabrice Bellard, and the corresponding
		MIT copyright notice can be found, and should remain, just below.
*/
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

#include "FFMPEG_Muxing.h"
#include <iostream>

//#define STREAM_DURATION   10.0
//#define STREAM_FRAME_RATE 60 /* 25 images/s */
//#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */
#define SCALE_FLAGS SWS_BICUBIC

void FFMPEG_Muxer::log_packet(const AVFormatContext* fmt_ctx, const AVPacket* pkt)
{
	AVRational* time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

	//printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
	//       av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
	//       av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
	//       av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
	//       pkt->stream_index);

	/* B.G: TODO: Replace with cout. */
}

int FFMPEG_Muxer::write_frame(AVFormatContext* fmt_ctx, const AVRational* time_base, AVStream* st, AVPacket* pkt)
{
	/* rescale output packet timestamp values from codec to stream timebase */
	av_packet_rescale_ts(pkt, *time_base, st->time_base);
	pkt->stream_index = st->index;

	/* Write the compressed frame to the media file. */
	log_packet(fmt_ctx, pkt);
	return av_interleaved_write_frame(fmt_ctx, pkt);
}

void FFMPEG_Muxer::add_stream(OutputStream* ost, AVFormatContext* oc,
	AVCodec ** codec,
	enum AVCodecID codec_id)
{
	AVCodecContext* c;
	int i;

	/* find the encoder */
	*codec = avcodec_find_encoder(codec_id);
	if (!(*codec)) {
		fprintf(stderr, "Could not find encoder for '%s'\n",
			avcodec_get_name(codec_id));
		std::cout << "ERROR";
	}

	ost->st = avformat_new_stream(oc, nullptr);
	if (!ost->st) {
		fprintf(stderr, "Could not allocate stream\n");
		std::cout << "ERROR";
	}
	ost->st->id = oc->nb_streams - 1;
	c = avcodec_alloc_context3(*codec);
	if (!c) {
		fprintf(stderr, "Could not alloc an encoding context\n");
		std::cout << "ERROR";
	}
	ost->enc = c;

	switch ((*codec)->type) {
	case AVMEDIA_TYPE_AUDIO:
	{
		c->sample_fmt = (*codec)->sample_fmts ?
			(*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
		c->bit_rate = 64000;
		c->sample_rate = 44100;
		if ((*codec)->supported_samplerates) {
			c->sample_rate = (*codec)->supported_samplerates[0];
			for (i = 0; (*codec)->supported_samplerates[i]; i++) {
				if ((*codec)->supported_samplerates[i] == 44100)
					c->sample_rate = 44100;
			}
		}
		c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
		c->channel_layout = AV_CH_LAYOUT_STEREO;
		if ((*codec)->channel_layouts) {
			c->channel_layout = (*codec)->channel_layouts[0];
			for (i = 0; (*codec)->channel_layouts[i]; i++) {
				if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
					c->channel_layout = AV_CH_LAYOUT_STEREO;
			}
		}
		c->channels = av_get_channel_layout_nb_channels(c->channel_layout);

		/* B.G: Non-standard type-conversion. */
		// AVRational = { .num = 1,.den = c->sample_rate }; (one day we can use designated initialisers, C++20)
		ost->st->time_base = av_make_q(1,c->sample_rate);
		break;
	}
	case AVMEDIA_TYPE_VIDEO:
	{
		c->codec_id = codec_id;

		c->bit_rate = 4000000;
		/* Resolution must be a multiple of two. */
		c->width = 1920;
		c->height = 1080;
		/* timebase: This is the fundamental unit of time (in seconds) in terms
		 * of which frame timestamps are represented. For fixed-fps content,
		 * timebase should be 1/framerate and timestamp increments should be
		 * identical to 1. */

		 /* B.G: Replaced initialisation with av_make_q, C++ doesn't support AVRational t = { int, int }*/
		ost->st->time_base = av_make_q(1,m_settings.m_framerate);
		c->time_base = ost->st->time_base;

		c->gop_size = 12; /* emit one intra frame every twelve frames at most */
		c->pix_fmt = m_settings.m_pixelFormat;
		if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
			/* just for testing, we also add B-frames */
			c->max_b_frames = 2;
		}
		if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
			/* Needed to avoid using macroblocks in which some coeffs overflow.
			 * This does not happen with normal video, it just happens here as
			 * the motion of the chroma plane does not match the luma plane. */
			c->mb_decision = 2;
		}
	}
	break;

	default:
		break;
	}
	/* Some formats want stream headers to be separate. */
	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

AVFrame* FFMPEG_Muxer::alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout,
	int sample_rate, int nb_samples)
{
	AVFrame* frame = av_frame_alloc();
	int ret;

	if (!frame) {
		fprintf(stderr, "Error allocating an audio frame\n");
		std::cout << "ERROR";
	}

	frame->format = sample_fmt;
	frame->channel_layout = channel_layout;
	frame->sample_rate = sample_rate;
	frame->nb_samples = nb_samples;

	if (nb_samples) {
		ret = av_frame_get_buffer(frame, 0);
		if (ret < 0) {
			fprintf(stderr, "Error allocating an audio buffer\n");
			std::cout << "ERROR";
		}
	}

	return frame;
}

void FFMPEG_Muxer::open_audio(AVFormatContext* oc, AVCodec* codec, OutputStream* ost, AVDictionary* opt_arg)
{
	int nb_samples{ 0 };
	int ret{ 0 };
	AVDictionary* opt{nullptr};
	AVCodecContext * c{ ost->enc };

	/* open it */
	av_dict_copy(&opt, opt_arg, 0);
	ret = avcodec_open2(c, codec, &opt);
	av_dict_free(&opt);
	if (ret < 0) {
		/* B.G: TODO: Replace with cout. */
		//fprintf(stderr, "Could not open audio codec: %s\n", av_err2str(ret));
		std::cout << "ERROR";
	}

	/* init signal generator */
	ost->t = 0;
	ost->tincr = 2 * M_PI * 110.0 / c->sample_rate;
	/* increment frequency by 110 Hz per second */
	ost->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

	if (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
		nb_samples = 10000;
	else
		nb_samples = c->frame_size;

	ost->frame = alloc_audio_frame(c->sample_fmt, c->channel_layout,
		c->sample_rate, nb_samples);
	ost->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_S16, c->channel_layout,
		c->sample_rate, nb_samples);

	/* copy the stream parameters to the muxer */
	ret = avcodec_parameters_from_context(ost->st->codecpar, c);
	if (ret < 0) {
		fprintf(stderr, "Could not copy the stream parameters\n");
		std::cout << "ERROR";
	}

	/* create resampler context */
	ost->swr_ctx = swr_alloc();
	if (!ost->swr_ctx) {
		fprintf(stderr, "Could not allocate resampler context\n");
		std::cout << "ERROR";
	}

	/* set options */
	av_opt_set_int(ost->swr_ctx, "in_channel_count", c->channels, 0);
	av_opt_set_int(ost->swr_ctx, "in_sample_rate", c->sample_rate, 0);
	av_opt_set_sample_fmt(ost->swr_ctx, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
	av_opt_set_int(ost->swr_ctx, "out_channel_count", c->channels, 0);
	av_opt_set_int(ost->swr_ctx, "out_sample_rate", c->sample_rate, 0);
	av_opt_set_sample_fmt(ost->swr_ctx, "out_sample_fmt", c->sample_fmt, 0);

	/* initialize the resampling context */
	if ((ret = swr_init(ost->swr_ctx)) < 0) {
		fprintf(stderr, "Failed to initialize the resampling context\n");
		std::cout << "ERROR";
	}
}

AVFrame* FFMPEG_Muxer::get_audio_frame(OutputStream* ost)
{
	AVFrame* frame{ ost->tmp_frame };
	int j{ 0 }, i{ 0 }, v{ 0 };
	int16_t* q = (int16_t*)frame->data[0];

	/* check if we want to generate more frames */
	/* B.G: Replaced initialisation with av_make_q, C++ doesn't support AVRational t = { int, int }*/
	if (av_compare_ts(ost->next_pts, ost->enc->time_base,
		m_settings.m_streamDuration, av_make_q(1, 1)) >= 0)
		return nullptr;

	for (j = 0; j < frame->nb_samples; j++) {
		v = (int)(sin(ost->t) * 10000);
		for (i = 0; i < ost->enc->channels; i++)
			* q++ = v;
		ost->t += ost->tincr;
		ost->tincr += ost->tincr2;
	}

	frame->pts = ost->next_pts;
	ost->next_pts += frame->nb_samples;

	return frame;
}

int FFMPEG_Muxer::write_audio_frame(AVFormatContext* oc, OutputStream* ost)
{
	AVCodecContext* c{ nullptr };
	AVPacket pkt = { 0 }; // data and size must be 0;
	AVFrame* frame{ nullptr };
	int ret{ 0 };
	int got_packet{ 0 };
	int dst_nb_samples{ 0 };

	av_init_packet(&pkt);
	c = ost->enc;

	/* B.G ISSUE: The pts of the next frame is updated without knowing if it was muxed in. */
	frame = get_audio_frame(ost);

	if (frame) {
		/* convert samples from native format to destination codec format, using the resampler */
			/* compute destination number of samples */
		dst_nb_samples = av_rescale_rnd(swr_get_delay(ost->swr_ctx, c->sample_rate) + frame->nb_samples,
			c->sample_rate, c->sample_rate, AV_ROUND_UP);
		av_assert0(dst_nb_samples == frame->nb_samples);

		/* when we pass a frame to the encoder, it may keep a reference to it
		 * internally;
		 * make sure we do not overwrite it here
		 */
		ret = av_frame_make_writable(ost->frame);
		if (ret < 0)
			std::cout << "ERROR";

		/* convert to destination format */
		ret = swr_convert(ost->swr_ctx,
			ost->frame->data, dst_nb_samples,
			(const uint8_t * *)frame->data, frame->nb_samples);
		if (ret < 0) {
			fprintf(stderr, "Error while converting\n");
			std::cout << "ERROR";
		}
		frame = ost->frame;

		/* B.G: Replaced Initialiser */
		AVRational timeBase;
		timeBase.num = 1;
		timeBase.den = c->sample_rate;
		frame->pts = av_rescale_q(ost->samples_count, timeBase, c->time_base);
		ost->samples_count += dst_nb_samples;
	}

	//ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
	/* B.G avcodec_encode_audio2 is deprecated! */
	if (frame != nullptr)
	{
		ret = avcodec_send_frame(c, frame);
		got_packet = !avcodec_receive_packet(c, &pkt);
	}
	else
	{
		/* No frame to encode. */
		return 1;
	}

	if (ret < 0) {
		/* B.G: TODO: Replace with cout. */
		// fprintf(stderr, "Error encoding audio frame: %s\n", av_err2str(ret));
		std::cout << "ERROR";
	}

	if (got_packet) {
		ret = write_frame(oc, &c->time_base, ost->st, &pkt);
		if (ret < 0) {
			/* B.G: TODO: Replace with cout. */
			// fprintf(stderr, "Error while writing audio frame: %s\n", av_err2str(ret));
			std::cout << "ERROR";
		}
	}

	return (frame || got_packet) ? 0 : 1;
}

AVFrame* FFMPEG_Muxer::alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
{
	AVFrame* picture;
	int ret;

	picture = av_frame_alloc();
	if (!picture)
		return nullptr;

	picture->format = pix_fmt;
	picture->width = width;
	picture->height = height;

	/* allocate the buffers for the frame data */
	ret = av_frame_get_buffer(picture, 32);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate frame data.\n");
		std::cout << "ERROR";
	}

	return picture;
}

void FFMPEG_Muxer::open_video(AVFormatContext* oc, AVCodec* codec, OutputStream* ost, AVDictionary* opt_arg)
{
	int ret{ 0 };
	AVCodecContext* c{ ost->enc };
	AVDictionary* opt{ nullptr };

	av_dict_copy(&opt, opt_arg, 0);

	/* open the codec */
	ret = avcodec_open2(c, codec, &opt);
	av_dict_free(&opt);
	if (ret < 0) {
		/* B.G: TODO: Replace with cout. */
		//fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));
		std::cout << "ERROR";
	}

	/* allocate and init a re-usable frame */
	ost->frame = alloc_picture(c->pix_fmt, c->width, c->height);
	if (!ost->frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		std::cout << "ERROR";
	}

	/* If the output format is not YUV420P, then a temporary YUV420P
	 * picture is needed too. It is then converted to the required
	 * output format. */
	ost->tmp_frame = nullptr;
	if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
		ost->tmp_frame = alloc_picture(AV_PIX_FMT_YUV420P, c->width, c->height);
		if (!ost->tmp_frame) {
			fprintf(stderr, "Could not allocate temporary picture\n");
			std::cout << "ERROR";
		}
	}

	/* copy the stream parameters to the muxer */
	ret = avcodec_parameters_from_context(ost->st->codecpar, c);
	if (ret < 0) {
		fprintf(stderr, "Could not copy the stream parameters\n");
		std::cout << "ERROR";
	}
}

void FFMPEG_Muxer::fill_yuv_image(AVFrame* pict, int frame_index,
	int width, int height)
{
	int x, y, i;

	i = frame_index;

	/* Y */
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
			pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;

	/* Cb and Cr */
	for (y = 0; y < height / 2; y++) {
		for (x = 0; x < width / 2; x++) {
			pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
			pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
		}
	}
}

AVFrame* FFMPEG_Muxer::get_video_frame(OutputStream* ost)
{
	AVCodecContext * c{ ost->enc };

	/* check if we want to generate more frames */
	/* B.G: Replaced Initialiser */
	if (av_compare_ts(ost->next_pts, c->time_base,
		m_settings.m_streamDuration, av_make_q(1,1)) >= 0)
		return nullptr;

	/* when we pass a frame to the encoder, it may keep a reference to it
	 * internally; make sure we do not overwrite it here */
	if (av_frame_make_writable(ost->frame) < 0)
		std::cout << "ERROR";

	if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
		/* as we only generate a YUV420P picture, we must convert it
		 * to the codec pixel format if needed */
		if (!ost->sws_ctx) {
			ost->sws_ctx = sws_getContext(c->width, c->height,
				AV_PIX_FMT_YUV420P,
				c->width, c->height,
				c->pix_fmt,
				SCALE_FLAGS, nullptr, nullptr, nullptr);
			if (!ost->sws_ctx) {
				fprintf(stderr,
					"Could not initialize the conversion context\n");
				std::cout << "ERROR";
			}
		}
		fill_yuv_image(ost->tmp_frame, ost->next_pts, c->width, c->height);
		sws_scale(ost->sws_ctx, (const uint8_t * const*)ost->tmp_frame->data,
			ost->tmp_frame->linesize, 0, c->height, ost->frame->data,
			ost->frame->linesize);
	}
	else {
		fill_yuv_image(ost->frame, (int)ost->next_pts, c->width, c->height);
	}

	ost->frame->pts = ost->next_pts++;

	return ost->frame;
}

int FFMPEG_Muxer::write_video_frame(AVFormatContext* oc, OutputStream* ost)
{
	int ret{ 0 };
	AVCodecContext* c{ nullptr };
	AVFrame* frame{ nullptr };
	int got_packet = 0;
	AVPacket pkt = { 0 };

	c = ost->enc;

	frame = get_video_frame(ost);

	av_init_packet(&pkt);

	/* encode the image */
	//ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
	/* B.G avcodec_encode_video2 is deprecated! */
	ret = avcodec_send_frame(c, frame);

	int packetReceiving{ -1 };
	if (frame != nullptr)
	{
		while (packetReceiving != 0) /* According to line 105 of avcodec.h, we repeat receive packet until nothing to find. */
		{
			avcodec_send_frame(c, frame);
			packetReceiving = avcodec_receive_packet(c, &pkt);
		}
		got_packet = 1;
	}
	else
	{
		return 1;
	}

	if (ret < 0) {
		/* B.G: TODO: Replace with cout. */
		//fprintf(stderr, "Error encoding video frame: %s\n", av_err2str(ret));
		std::cout << "ERROR";
	}

	if (got_packet) {
		ret = write_frame(oc, &c->time_base, ost->st, &pkt);
	}
	else {
		ret = 0;
	}

	if (ret < 0) {
		/* B.G: TODO: Replace with cout. */
		//fprintf(stderr, "Error while writing video frame: %s\n", av_err2str(ret));
		std::cout << "ERROR";
	}

	return (frame || got_packet) ? 0 : 1;
}

void FFMPEG_Muxer::close_stream(AVFormatContext* oc, OutputStream* ost)
{
	avcodec_free_context(&ost->enc);
	av_frame_free(&ost->frame);
	av_frame_free(&ost->tmp_frame);
	sws_freeContext(ost->sws_ctx);
	swr_free(&ost->swr_ctx);
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		printf("usage: %s output_file\n"
			"API example program to output a media file with libavformat.\n"
			"This program generates a synthetic audio and video stream, encodes and\n"
			"muxes them into a file named output_file.\n"
			"The output format is automatically guessed according to the file extension.\n"
			"Raw images can also be output by using '%%d' in the filename.\n"
			"\n", argv[0]);
		return 1;
	}

	// TODO: Set flags somewhere. 
	//for (int i = 2; i + 1 < argc; i += 2) {
	//	if (!strcmp(argv[i], "-flags") || !strcmp(argv[i], "-fflags"))
	//		av_dict_set(&opt, argv[i] + 1, argv[i + 1], 0);
	//}
	MuxerSettings settings{};
	settings.m_fileName = argv[1];
	FFMPEG_Muxer mux{};

	mux.initialise(settings);
	mux.run();
	mux.deinitialise();

	return 0;
}

bool FFMPEG_Muxer::initialise(MuxerSettings settings)
{
	m_settings = settings;

	/* allocate the output media context */
	avformat_alloc_output_context2(&oc, nullptr, nullptr, settings.m_fileName.c_str());
	if (!oc) { /* B.G: Assumes that failure to allocate a context is because the format wasn't found from the 
			           file extension. */
		printf("Could not deduce output format from file extension: using MPEG.\n");
		avformat_alloc_output_context2(&oc, nullptr, "mpeg", settings.m_fileName.c_str());
	}
	if (!oc)
		return false;

	fmt = oc->oformat;

	/* Add the audio and video streams using the default format codecs
	 * and initialize the codecs. */
	if (fmt->video_codec != AV_CODEC_ID_NONE) { /* <- B.G TODO: Check if this means that only audio/video can be encoded here.*/
		add_stream(&video_st, oc, &video_codec, fmt->video_codec);
		have_video = 1;
		encode_video = 1;
	}
	if (fmt->audio_codec != AV_CODEC_ID_NONE) {
		add_stream(&audio_st, oc, &audio_codec, fmt->audio_codec);
		have_audio = 1;
		encode_audio = 1;
	}

	/* Now that all the parameters are set, we can open the audio and
	 * video codecs and allocate the necessary encode buffers. */
	if (have_video)
		open_video(oc, video_codec, &video_st, opt);

	if (have_audio)
		open_audio(oc, audio_codec, &audio_st, opt);

	/* B.G: Tells us about the output format. */
	av_dump_format(oc, 0, settings.m_fileName.c_str(), 1);

	/* open the output file, if needed */
	if (!(fmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&oc->pb, settings.m_fileName.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0) {
			/* B.G: TODO: Replace with cout. */
			//fprintf(stderr, "Could not open '%s': %s\n", filename, av_err2str(ret));
			return false;
		}
	}

	/* Write the stream header, if any. */
	ret = avformat_write_header(oc, &opt);
	if (ret < 0) {
		/* B.G: TODO: Replace with cout. */
		//fprintf(stderr, "Error occurred when opening output file: %s\n", av_err2str(ret));
		return false;
	}
}

void FFMPEG_Muxer::run()
{
	/* B.G : This compares the time elapsed in the encoded video and audio, to determine
		 which stream to write to each time.*/
	while (encode_video || encode_audio) {
		/* select the stream to encode */
		if (encode_video && (!encode_audio || av_compare_ts(video_st.next_pts, video_st.enc->time_base,
			audio_st.next_pts, audio_st.enc->time_base) <= 0)) {
			encode_video = !write_video_frame(oc, &video_st);
		}
		else {
			encode_audio = !write_audio_frame(oc, &audio_st);
		}
	}
}

void FFMPEG_Muxer::deinitialise()
{
	/* Write the trailer, if any. The trailer must be written before you
	 * close the CodecContexts open when you wrote the header; otherwise
	 * av_write_trailer() may try to use memory that was freed on
	 * av_codec_close(). */
	av_write_trailer(oc);

	/* Close each codec. */
	if (have_video)
		close_stream(oc, &video_st);
	if (have_audio)
		close_stream(oc, &audio_st);

	if (!(fmt->flags & AVFMT_NOFILE))
		/* Close the output file. */
		avio_closep(&oc->pb);

	/* free the stream */
	avformat_free_context(oc);
}
