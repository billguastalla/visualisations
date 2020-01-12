/*
	TODO:
		- Provide an interface appropriate for the visualisation project.
		- Handle all exit(1) statements, and de-allocate appropriately,
		  with errors.
		- Replace all C-like fprint statements, initialise all variables,
		  erase global variables, make member variables and handle carefully.
		- Write tests for rendering sample video in all formats, in unittests.
		- Check with FFMPEG or StackOverflow/CodeReview that the code is appropriate.

	Structure: (main components are..)
		-> Top level interface:
			-> Add audio stream
			-> Add video stream

		Streams:
			-> Allow external input of audio via interface.
			-> Allow external input of video via interface.

		- Publish. Reasons:
			-> Want to be able to run code without exiting the program on failure.
			-> Want to be able to re-run muxer multiple times with different settings.
			-> Want to be able to see the structure clearly.
			-> Want to compile this in the C++11 standard without warnings.
				(Note: Disable MSVC compiler extensions before publishing as a separate repo!)


		Integration into Visualisations:
			-> Need the muxer to be the caller to gl rendering and audio acquisition,
				since it must coordinate picking up of AVFrames.
				-> Need to decide the policy on what stops the render:

*/
/*
	Bill Guastalla 2020: MUXING:
		This is a port, refactored from C to C++11, for a code example found in the FFMPEG
		documentation. The original work is by Fabrice Bellard, and the corresponding
		copyright notice can be found, and should remain, just below.
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
	/* B.G: Replaced with cout. */
	/*std::cout
		<< "pts:" << pkt->pts << " "
		<< "pts_time:" << av_q2d(*time_base) * pkt->pts << " "
		<< "dts:" << pkt->dts << " "
		<< "dts_time:" << av_q2d(*time_base) * pkt->dts << " "
		<< "duration:" << pkt->duration << " "
		<< "duration_time:" << av_q2d(*time_base) * pkt->duration << " "
		<< "stream_index:" << pkt->stream_index << std::endl;*/
		//printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
		//       av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
		//       av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
		//       av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
		//       pkt->stream_index);
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

bool FFMPEG_Muxer::add_stream(OutputStream* ost, AVFormatContext* oc, AVCodec** codec, enum AVCodecID codec_id)
{
	AVCodecContext* c{ nullptr };

	/* find the encoder */
	*codec = avcodec_find_encoder(codec_id);
	if (!(*codec)) {

		std::string error{ "Adding Stream: Could not find encoder for " };
		error = error.append(avcodec_get_name(codec_id));
		reportError(error);
		return false;
	}
	else /* B.G: Streams get to hold a pointer to their codec now. */
		ost->p_codec = *codec;

	ost->st = avformat_new_stream(oc, nullptr);

	if (!ost->st) {
		reportError("Adding Stream: Could not allocate stream.");
		return false;
	}
	else
		m_allocationStage = (AllocationStage)(m_allocationStage +
			AllocationStage::OutputStream_Stream);

	ost->st->id = (oc->nb_streams - 1);
	c = avcodec_alloc_context3(*codec);
	if (!c)
	{
		reportError("Adding Stream: Could not allocate an encoding context");
		deallocate();
		return false;
	}
	else
		m_allocationStage = (AllocationStage)(m_allocationStage +
			AllocationStage::OutputStream_CodecContext);

	ost->enc = c;

	switch ((*codec)->type) {
	case AVMEDIA_TYPE_AUDIO:
	{
		c->sample_fmt = (*codec)->sample_fmts ?
			(*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
		c->bit_rate = m_settings.m_audioBitRate;
		c->sample_rate = m_settings.m_audioDefaultSampleRate;
		if ((*codec)->supported_samplerates) {
			c->sample_rate = (*codec)->supported_samplerates[0];
			for (int i = 0; (*codec)->supported_samplerates[i]; i++) {
				if ((*codec)->supported_samplerates[i] == 44100)
					c->sample_rate = 44100;
			}
		}
		c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
		c->channel_layout = AV_CH_LAYOUT_STEREO;
		if ((*codec)->channel_layouts) {
			c->channel_layout = (*codec)->channel_layouts[0];
			for (int i = 0; (*codec)->channel_layouts[i]; i++) {
				if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
					c->channel_layout = AV_CH_LAYOUT_STEREO;
			}
		}
		c->channels = av_get_channel_layout_nb_channels(c->channel_layout);

		/* B.G: Non-standard type-conversion. */
		// AVRational = { .num = 1,.den = c->sample_rate }; (one day we can use designated initialisers, C++20)
		ost->st->time_base = av_make_q(1, c->sample_rate);
		break;
	}
	case AVMEDIA_TYPE_VIDEO:
	{
		c->codec_id = codec_id;

		c->bit_rate = m_settings.m_videoBitRate;
		/* Resolution must be a multiple of two. */
		c->width = m_settings.m_frameWidth;
		c->height = m_settings.m_frameHeight;
		/* timebase: This is the fundamental unit of time (in seconds) in terms
		 * of which frame timestamps are represented. For fixed-fps content,
		 * timebase should be 1/framerate and timestamp increments should be
		 * identical to 1. */

		 /* B.G: Replaced initialisation with av_make_q, C++ doesn't support AVRational t = { int, int }*/
		ost->st->time_base = av_make_q(1, m_settings.m_framerate);
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


	/* B.G: If we added the stream it's initialised, and not finished (replacement for have_audio/encode_audio */
	ost->m_initialised = true;
	ost->m_finished = false;

	return true;
}

AVFrame* FFMPEG_Muxer::alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout,
	int sample_rate, int nb_samples)
{
	AVFrame* frame{ av_frame_alloc() };
	int ret{ 0 };

	if (!frame) {
		reportError("Error allocating an audio frame.");
		return nullptr;
	}
	else
		m_allocationStage = (AllocationStage)(m_allocationStage + AllocationStage::OutputStream_AudioFrame);

	frame->format = sample_fmt;
	frame->channel_layout = channel_layout;
	frame->sample_rate = sample_rate;
	frame->nb_samples = nb_samples;

	if (nb_samples) {
		ret = av_frame_get_buffer(frame, 0);
		if (ret < 0) {
			reportError("Error allocating an audio buffer.", ret);
			return nullptr;
		}
	}
	return frame;
}

void FFMPEG_Muxer::open_audio(AVFormatContext* oc, AVCodec* codec, OutputStream* ost, AVDictionary* opt_arg)
{
	int nb_samples{ 0 };
	int ret{ 0 };
	AVDictionary* opt{ nullptr };
	AVCodecContext* c{ ost->enc };

	/* open it */
	av_dict_copy(&opt, opt_arg, 0);
	ret = avcodec_open2(c, codec, &opt);
	av_dict_free(&opt);
	if (ret < 0) {
		/* B.G: NOTE: This can happen if an unsupported sample rate is provided,
						when the supported sample rates are not shown for the codec. */
		reportError("Could not open audio codec.", ret);
	}

	/* init signal generator */
	ost->t = 0;
	ost->tincr = (float)(2.0 * M_PI * 110.0 / c->sample_rate);
	/* increment frequency by 110 Hz per second */
	ost->tincr2 = (float)(2.0 * M_PI * 110.0 / c->sample_rate / c->sample_rate);

	/* B.G: If we can send large frames then do it. */
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
		reportError("Could not copy the stream parameters\n", ret);
	}

	/* create resampler context */
	ost->swr_ctx = swr_alloc();
	if (!ost->swr_ctx) {
		reportError("Could not allocate the resampler context\n");
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
		reportError("Failed to initialize the resampling context\n", ret);
	}
}

AVFrame* FFMPEG_Muxer::get_audio_frame(OutputStream* ost)
{
	AVFrame* frame{ ost->tmp_frame };
	int j{ 0 }, i{ 0 }, v{ 0 };
	int16_t* q = (int16_t*)frame->data[0];

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
		dst_nb_samples = (int)av_rescale_rnd(
			swr_get_delay(ost->swr_ctx, c->sample_rate) + (int64_t)frame->nb_samples,
			(int64_t)c->sample_rate,
			(int64_t)c->sample_rate,
			AV_ROUND_UP);
		av_assert0(dst_nb_samples == frame->nb_samples);

		/* when we pass a frame to the encoder, it may keep a reference to it
		 * internally;
		 * make sure we do not overwrite it here
		 */
		ret = av_frame_make_writable(ost->frame);
		if (ret < 0)
		{
			reportError("Writing Audio Frame: Could not make frame writable", ret);
		}

		/* convert to destination format */
		ret = swr_convert(ost->swr_ctx,
			ost->frame->data, dst_nb_samples,
			(const uint8_t * *)frame->data, frame->nb_samples);
		if (ret < 0) {
			reportError("Writing Audio Frame: Error while converting frame \n", ret);
		}
		frame = ost->frame;

		/* B.G: Inserted av_make_q().  */
		frame->pts = av_rescale_q(ost->samples_count, av_make_q(1, c->sample_rate), c->time_base);
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
		reportError("Writing Audio Frame: Error encoding audio frame: ", ret);

	}

	if (got_packet) {
		ret = write_frame(oc, &c->time_base, ost->st, &pkt);
		if (ret < 0) {
			reportError("Writing Audio Frame: Error while writing audio frame: ", ret);
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
	/* B.G: UNKNOWN: Find out why 32 needs to be passed into alignment parameter. */
	ret = av_frame_get_buffer(picture, 32);
	if (ret < 0) {
		reportError("Could not allocate frame data.", ret);
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
		reportError("Opening Video Stream: Could not open video codec. " \
			"Possible Causes: Video parameters incompatible with codec.", ret);
	}

	/* allocate and init a re-usable frame */
	ost->frame = alloc_picture(c->pix_fmt, c->width, c->height);
	if (!ost->frame) {
		reportError("Opening Video Stream: Could not allocate video frame.", ret);
	}

	/* If the output format is not YUV420P, then a temporary YUV420P
	 * picture is needed too. It is then converted to the required
	 * output format. */
	ost->tmp_frame = nullptr;
	if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
		ost->tmp_frame = alloc_picture(AV_PIX_FMT_YUV420P, c->width, c->height);
		if (!ost->tmp_frame) {
			reportError("Opening Video Stream: Could not allocate temporary picture.");
		}
	}

	/* copy the stream parameters to the muxer */
	ret = avcodec_parameters_from_context(ost->st->codecpar, c);
	if (ret < 0) {
		reportError("Opening Video Stream: Could not copy the stream parameters.", ret);
	}
}

void FFMPEG_Muxer::fill_yuv_image(AVFrame* pict, int frame_index,
	int width, int height)
{
	int x{ 0 }, y{ 0 }, i{ 0 };
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
	AVCodecContext* c{ ost->enc };

	/* when we pass a frame to the encoder, it may keep a reference to it
	 * internally; make sure we do not overwrite it here */
	if (int r = av_frame_make_writable(ost->frame) < 0)
		reportError("Getting Video Frame: Could not make frame writable.", r);

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
				reportError("Getting Video Frame: Could not initialize the conversion context.");
			}
		}
		fill_yuv_image(
			ost->tmp_frame,
			(int)ost->next_pts,
			c->width,
			c->height);
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
	int got_packet = 0;
	AVCodecContext* c{ ost->enc };
	AVPacket pkt = { 0 };
	av_init_packet(&pkt);

	/* encode the image */
	//ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
	/* B.G avcodec_encode_video2 is deprecated! */

	AVFrame* frame{ nullptr };

	/* B.G:
		We're getting an issue here:
		"non-strictly-monotonic PTS". Implies that we have sent the same frame multiple times.
		API says that we should send more frames. Need to check if this is okay in
		terms of synchronising audio & video (i.e. comparison in each timebase is done
		outside this function.)
	*/
	bool packetReceived{ false };

	while (!packetReceived)
	{
		/* B.G Relying on fact that get_video_frame() updates pts. */
		frame = get_video_frame(ost);
		if (frame != nullptr)
		{
			ret = avcodec_send_frame(c, frame);
		}
		else
			return 1;
		ret = avcodec_receive_packet(c, &pkt);
		if (ret == 0)
			packetReceived = true;
	}
	/* According to line 105 of avcodec.h, we repeat receive packet until nothing to find. */
	got_packet = 1;

	if (ret < 0) {
		reportError("Writing Video Frame: Error encoding video frame: ", ret);
		return 1;
	}

	if (got_packet) {
		ret = write_frame(oc, &c->time_base, ost->st, &pkt);
	}
	if (ret < 0) {
		reportError("Writing Video Frame: Error while writing video frame.", ret);
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
	ost->m_initialised = false;
}

bool FFMPEG_Muxer::deallocate()
{
	return false;
}

int main(int argc, char** argv)
{
	MuxerSettings settings{};

	if (argc < 2) {
		printf("usage: %s output_file\n"
			"API example program to output a media file with libavformat.\n"
			"This program generates a synthetic audio and video stream, encodes and\n"
			"muxes them into a file named output_file.\n"
			"The output format is automatically guessed according to the file extension.\n"
			"Raw images can also be output by using '%%d' in the filename.\n"
			"\n", argv[0]);
		settings.m_fileName = std::string{ "out_no_passedname.mkv" };
	}
	else
		settings.m_fileName = argv[1];

	// TODO: Set flags somewhere. 
	//for (int i = 2; i + 1 < argc; i += 2) {
	//	if (!strcmp(argv[i], "-flags") || !strcmp(argv[i], "-fflags"))
	//		av_dict_set(&opt, argv[i] + 1, argv[i + 1], 0);
	//}
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
		reportError("Initialising: Could not deduce output format from file extension: using MPEG.\n");
		avformat_alloc_output_context2(&oc, nullptr, "mpeg", settings.m_fileName.c_str());
	}
	if (!oc)
	{
		reportError("Initialising: Could not allocate output context.");
		return false;
	}
	else /* B.G: TODO: Use a better way to add flags*/
		m_allocationStage = (AllocationStage)(m_allocationStage +
			AllocationStage::Output_FormatContext);

	fmt = oc->oformat;

	/* B.G: TODO: This is */

	/* Add the audio and video streams using the default format codecs
	 * and initialize the codecs. */
	if (fmt->video_codec != AV_CODEC_ID_NONE) { /* <- B.G TODO: Check if this means that only audio/video can be encoded here.*/
		for (int i = 0; i < 10; ++i)
		{
			OutputStream videoStream{};
			add_stream(&videoStream, oc, &video_codec, fmt->video_codec);
			m_streams.push_back(videoStream);
		}
	}
	if (fmt->audio_codec != AV_CODEC_ID_NONE) {
		for (int i = 0; i < 10; ++i)
		{
			OutputStream audioStream{};
			add_stream(&audioStream, oc, &audio_codec, fmt->audio_codec);
			m_streams.push_back(audioStream);
		}
	}

	/* Now that all the parameters are set, we can open the audio and
	 * video codecs and allocate the necessary encode buffers. */
	for (auto i = m_streams.begin(); i != m_streams.end(); ++i)
	{
		if (i->m_initialised)
		{
			/* B.G: Initialises codec context for stream */
			if (i->p_codec->type == AVMediaType::AVMEDIA_TYPE_VIDEO)
				open_video(oc, video_codec, &*i, opt);
			else if (i->p_codec->type == AVMediaType::AVMEDIA_TYPE_AUDIO)
				open_audio(oc, audio_codec, &*i, opt);
		}
	}

	/* B.G: Tells us about the output format. */
	av_dump_format(oc, 0, settings.m_fileName.c_str(), 1);

	/* open the output file, if needed */
	if (!(fmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&oc->pb, settings.m_fileName.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0) {
			std::string errorStr{ "Initialising: Could not open file :" };
			errorStr += filename;
			reportError(errorStr, ret);
			return false;
		}
	}

	/* Write the stream header, if any. */
	ret = avformat_write_header(oc, &opt);
	if (ret < 0) {
		std::string errorStr{ "Initialising: Error occurred when opening output file " };
		errorStr += filename;
		errorStr += ".";
		reportError(errorStr, ret);
		return false;
	}

	return true;
}

void FFMPEG_Muxer::run()
{
	/* B.G : This compares the time elapsed in the encoded video and audio, to determine
		 which stream to write to each time.*/
	while (OutputStream::streamsUnfinshed(m_streams, m_settings.m_streamDuration))
	{
		/* B.G: select the stream to encode */
		OutputStream& stream{ OutputStream::nextStream(m_streams) };
		/* B.G: Alternative here would be polymorphism. */
		if (stream.p_codec->type == AVMediaType::AVMEDIA_TYPE_AUDIO)
			write_audio_frame(oc, &(stream)); /* B.G:TODO: Ugly dereferencing, fix in function args, or iterator return type.*/
		else if (stream.p_codec->type == AVMediaType::AVMEDIA_TYPE_VIDEO)
			write_video_frame(oc, &(stream));
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
	for (auto i = m_streams.begin(); i != m_streams.end(); ++i)
		if (i->m_initialised)
			close_stream(oc, &*i);

	if (!(fmt->flags & AVFMT_NOFILE))
		/* Close the output file. */
		avio_closep(&oc->pb);

	/* free the stream */
	avformat_free_context(oc);
}

void FFMPEG_Muxer::reportError(std::string description, int errorNum)
{
	std::cout << "FFMPEG Muxing: " << description;
	if (errorNum != 0)
	{
		char errorStr[255];
		av_make_error_string(errorStr, 255, errorNum);
		std::string error{ errorStr };
		std::cout
			<< "\n\tAPI Says:\t" << error
			<< "\n\tError Code:\t" << errorNum;
	}
	std::cout << std::endl;
}

void OutputStream::deallocate()
{
	//if (m_allocation & StreamAllocation::CodecContext)
	//	avcodec_free_context(&enc);
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
