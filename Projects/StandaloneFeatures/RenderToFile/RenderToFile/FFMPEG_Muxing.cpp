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
				-> Need to decide the policy on what stops the render
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
#include "FFMPEG_AudioStream.h"
#include "FFMPEG_VideoStream.h"

#include <iostream>

 //#define STREAM_DURATION   10.0
 //#define STREAM_FRAME_RATE 60 /* 25 images/s */
 //#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */

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

int FFMPEG_Muxer::write_frame(AVFormatContext* fmt_ctx, const AVRational* time_base, AVStream* m_avstream, AVPacket* pkt)
{
	/* rescale output packet timestamp values from codec to stream timebase */
	av_packet_rescale_ts(pkt, *time_base, m_avstream->time_base);
	pkt->stream_index = m_avstream->index;

	/* Write the compressed frame to the media file. */
	log_packet(fmt_ctx, pkt);
	return av_interleaved_write_frame(fmt_ctx, pkt);
}

bool FFMPEG_Muxer::add_stream(FFMPEG_Stream* stream, AVFormatContext* oc, AVCodec** codec, enum AVCodecID codec_id)
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
		stream->p_codec = *codec;

	stream->m_avstream = avformat_new_stream(oc, nullptr);

	if (!stream->m_avstream) {
		reportError("Adding Stream: Could not allocate stream.");
		return false;
	}
	else
		m_allocationStage = (AllocationStage)(m_allocationStage +
			AllocationStage::OutputStream_Stream);

	stream->m_avstream->id = (oc->nb_streams - 1);
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

	stream->m_avcodecEncoderContext = c;

	switch ((*codec)->type)
	{
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
		stream->m_avstream->time_base = av_make_q(1, c->sample_rate);
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
		stream->m_avstream->time_base = av_make_q(1, m_settings.m_framerate);
		c->time_base = stream->m_avstream->time_base;

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
	stream->m_initialised = true;
	stream->m_finished = false;

	return true;
}

bool FFMPEG_Muxer::deallocate()
{
	return false;
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
			FFMPEG_Stream * videoStream{new FFMPEG_VideoStream{}};
			add_stream(videoStream, oc, &video_codec, fmt->video_codec);
			m_streams.push_back(videoStream);
		}
	}
	if (fmt->audio_codec != AV_CODEC_ID_NONE) {
		for (int i = 0; i < 10; ++i)
		{
			FFMPEG_Stream* audioStream{ new FFMPEG_AudioStream{} };
			add_stream(audioStream, oc, &audio_codec, fmt->audio_codec);
			m_streams.push_back(audioStream);
		}
	}
	/* Now that all the parameters are set, we can open the audio and
	 * video codecs and allocate the necessary encode buffers. */
	for (auto i = m_streams.begin(); i != m_streams.end(); ++i)
	{
		if ((*i)->m_initialised)
		{
			///* B.G: Initialises codec context for stream */
			//if (i->p_codec->type == AVMediaType::AVMEDIA_TYPE_VIDEO)
			(*i)->open(oc, opt);
			//else if (i->p_codec->type == AVMediaType::AVMEDIA_TYPE_AUDIO)
			//	open_audio(oc, audio_codec, &*i, opt);
		}
	}
	/* B.G: Tells us about the output format of each stream we're writing. */
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
	while (FFMPEG_Stream::streamsUnfinished(m_streams, m_settings.m_streamDuration))
	{
		/* B.G: select the stream to encode */
		FFMPEG_Stream * stream{ FFMPEG_Stream::nextStream(m_streams) };
		/* B.G: One alternative here would be polymorphism. */

		AVPacket packet{};
		if (stream->buildPacket(packet, oc))
		{
			write_frame(oc, &stream->m_avcodecEncoderContext->time_base, stream->m_avstream, &packet);
		}
		//if (stream.p_codec->type == AVMediaType::AVMEDIA_TYPE_AUDIO)
		//	write_audio_frame(oc, &(stream));
		//else if (stream.p_codec->type == AVMediaType::AVMEDIA_TYPE_VIDEO)
		//	write_video_frame(oc, &(stream));
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
		if ((*i)->m_initialised)
			(*i)->close();
	m_streams.clear();

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
