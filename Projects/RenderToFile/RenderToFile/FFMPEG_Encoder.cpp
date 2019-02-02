#pragma once
#include "FFMPEG_Encoder.h"

#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
/* Adapted from: https://github.com/cirosantilli/cpp-cheat/blob/19044698f91fefa9cb75328c44f7a487d336b541/ffmpeg/encode.c */

void FFMPEG_Encoder::ffmpeg_encoder_set_frame_yuv_from_rgb(uint8_t *rgb) {
	const int in_linesize[1] = { 4 * m_AVCodecContext->width };
	m_swsContext = sws_getCachedContext(m_swsContext,
		m_AVCodecContext->width, m_AVCodecContext->height, AV_PIX_FMT_RGB32,
		m_AVCodecContext->width, m_AVCodecContext->height, AV_PIX_FMT_YUV420P,
		0, NULL, NULL, NULL);
	sws_scale(m_swsContext, (const uint8_t * const *)&rgb, in_linesize, 0,
		m_AVCodecContext->height, m_AVFrame->data, m_AVFrame->linesize);
}

FFMPEG_Encoder::FFMPEG_Encoder()
	: m_started{ false }
{
}

FFMPEG_Encoder::StartResult FFMPEG_Encoder::ffmpeg_encoder_start(const char *filename, AVCodecID codec_id, int fps, int width, int height)
{
	if (!m_started)
	{
		AVCodec *codec;
		int ret;
		avcodec_register_all();
		codec = avcodec_find_encoder(codec_id);
		if (!codec) {
			fprintf(stderr, "Codec not found\n");
			exit(1);
			return StartResult::CodecNotFound;
		}
		m_AVCodecContext = avcodec_alloc_context3(codec);
		if (!m_AVCodecContext) {
			fprintf(stderr, "Could not allocate video codec context\n");
			exit(1);
			return StartResult::ContextAllocationFailed;
		}
		m_AVCodecContext->bit_rate = 400000;
		m_AVCodecContext->width = width;
		m_AVCodecContext->height = height;
		m_AVCodecContext->time_base.num = 1;
		m_AVCodecContext->time_base.den = fps;
		m_AVCodecContext->gop_size = 10;
		m_AVCodecContext->max_b_frames = 1;
		m_AVCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
		if (codec_id == AV_CODEC_ID_H264)
			av_opt_set(m_AVCodecContext->priv_data, "preset", "slow", 0);
		if (avcodec_open2(m_AVCodecContext, codec, NULL) < 0) {
			fprintf(stderr, "Could not open codec\n");
			exit(1);
			return StartResult::CodecUnopenable;
		}
		m_file = fopen(filename, "wb");
		if (!m_file) {
			fprintf(stderr, "Could not open %s\n", filename);
			exit(1);
			return StartResult::FileUnopenable;
		}
		m_AVFrame = av_frame_alloc();
		if (!m_AVFrame) {
			fprintf(stderr, "Could not allocate video frame\n");
			exit(1);
			return StartResult::AVFrameAllocationFailed;
		}
		m_AVFrame->format = m_AVCodecContext->pix_fmt;
		m_AVFrame->width = m_AVCodecContext->width;
		m_AVFrame->height = m_AVCodecContext->height;

		ret = av_image_alloc(m_AVFrame->data, m_AVFrame->linesize, m_AVCodecContext->width, m_AVCodecContext->height, m_AVCodecContext->pix_fmt, 32);
		if (ret < 0) {
			fprintf(stderr, "Could not allocate raw picture buffer\n");
			exit(1);
			return StartResult::PictureBufferAllocationFailed;
		}
	}
	else
		return StartResult::EncoderAlreadyStarted;
	m_started = true;
}

void FFMPEG_Encoder::ffmpeg_encoder_render_frame()
{
	if (m_started)
	{
		/* BILL: Use PPM if PNG is not available: We can remove this if we just want video, good to know.. */
		//if (output_formats & PPM_BIT) {
		//	snprintf(filename, SCREENSHOT_MAX_FILENAME, "tmp.%d.ppm", nframes);
		//	screenshot_ppm(filename, width, height, &pixels);
		//}
		//if (output_formats & LIBPNG_BIT) {
		//	snprintf(filename, SCREENSHOT_MAX_FILENAME, "tmp.%d.png", nframes);
		//	screenshot_png(filename, width, height, &pixels, &png_bytes, &png_rows);
		//}
		m_AVFrame->pts = m_frameCount;
		ffmpeg_encoder_glread_rgb(&m_rgb, &m_pixels, m_AVCodecContext->width, m_AVCodecContext->height);
		ffmpeg_encoder_encode_frame(m_rgb);

		/* We've rendered a frame. */
		++m_frameCount;
		if (m_maxFrames != -1)
		{
			/* If we're done, finish: */
			if (m_maxFrames <= m_frameCount)
				ffmpeg_encoder_finish();
		}
	}
}

FFMPEG_Encoder::FinishResult FFMPEG_Encoder::ffmpeg_encoder_finish()
{
	if (m_started)
	{
		uint8_t endcode[] = { 0, 0, 1, 0xb7 };
		int got_output, ret;
		do {
			fflush(stdout);
			ret = avcodec_encode_video2(m_AVCodecContext, &m_AVPacket, NULL, &got_output);
			if (ret < 0) {
				fprintf(stderr, "Error encoding m_AVFrame\n");
				exit(1);

			}
			if (got_output) {
				fwrite(m_AVPacket.data, 1, m_AVPacket.size, m_file);
				av_packet_unref(&m_AVPacket);
			}
		} while (got_output);

		fwrite(endcode, 1, sizeof(endcode), m_file);
		fclose(m_file);

		avcodec_close(m_AVCodecContext);
		av_free(m_AVCodecContext);
		av_freep(&m_AVFrame->data[0]);
		av_frame_free(&m_AVFrame);

		m_started = false;
	}
}

void  FFMPEG_Encoder::ffmpeg_encoder_encode_frame(uint8_t *rgb) {
	int ret, got_output;
	ffmpeg_encoder_set_frame_yuv_from_rgb(rgb);
	av_init_packet(&m_AVPacket);
	m_AVPacket.data = NULL;
	m_AVPacket.size = 0;

	/* BILL - avcodec_encode_video2 is depreacated, so you'll need to do some swaps.
	//avcodec_send_frame(m_AVCodecContext, m_AVFrame);
	//avcodec_receive_packet(m_AVCodecContext, &m_AVPacket);
	*/
	ret = avcodec_encode_video2(m_AVCodecContext, &m_AVPacket, m_AVFrame, &got_output);
	if (ret < 0) {
		fprintf(stderr, "Error encoding m_AVFrame\n");
		exit(1);
	}
	if (got_output) {
		fwrite(m_AVPacket.data, 1, m_AVPacket.size, m_file);
		av_packet_unref(&m_AVPacket);
	}
}

void FFMPEG_Encoder::ffmpeg_encoder_glread_rgb(uint8_t **rgb, GLubyte **pixels, unsigned int width, unsigned int height) {
	size_t i, j, k, cur_gl, cur_rgb, nvals;
	const size_t format_nchannels = 4;
	nvals = format_nchannels * width * height;
	*pixels = (GLubyte*)realloc(*pixels, nvals * sizeof(GLubyte));
	*rgb = (uint8_t*)realloc(*rgb, nvals * sizeof(uint8_t));

	/* Get RGBA to align to 32 bits instead of just 24 for RGB. May be faster for FFmpeg. */
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, *pixels);

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			cur_gl = format_nchannels * (width * (height - i - 1) + j);
			cur_rgb = format_nchannels * (width * i + j);
			for (k = 0; k < format_nchannels; k++)
				(*rgb)[cur_rgb + k] = (*pixels)[cur_gl + k];
		}
	}
}