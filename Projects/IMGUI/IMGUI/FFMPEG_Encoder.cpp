#pragma once
#include "FFMPEG_Encoder.h"

/* Adapted from: https://github.com/cirosantilli/cpp-cheat/blob/19044698f91fefa9cb75328c44f7a487d336b541/ffmpeg/encode.c */

void FFMPEG_Encoder::rgbIntoFrame(AVFrame * frame, const AVCodecContext * ctx, const std::vector<uint8_t> & rgbData)
{
	const uint8_t * rgbPtr = &rgbData[0];

	/* This is the number of bytes in a horizontal line of a frame. */
	const int in_linesize[1] = { 4 * ctx->width };
	m_swsContext = sws_getCachedContext(m_swsContext,
		ctx->width, ctx->height, AV_PIX_FMT_RGBA,
		ctx->width, ctx->height, AV_PIX_FMT_YUV420P,
		0, nullptr, nullptr, nullptr);
	/* sws_scale needs a constant pointer to a pointer of the first element of the rgb data. */
	sws_scale(m_swsContext, (const uint8_t * const *)&rgbPtr, in_linesize, 0,
		ctx->height, frame->data, frame->linesize);
}

FFMPEG_Encoder::FFMPEG_Encoder()
	:
	m_started{ false },
	m_AVCodecContext{nullptr},
	m_AVFrame{nullptr},
	m_AVPacket{},
	m_currentFrame{0},
	m_maxFrames{ -1 }
{
}

FFMPEG_Encoder::StartResult FFMPEG_Encoder::ffmpeg_encoder_start(const char *filename, AVCodecID codec_id, int fps, int width, int height)
{
	/* Commented code is attempts to create mp4 containers & add framewriting for audio & video together. */


	/* Not working yet: MP4 Container Creation
		-> Extracted from: https://stackoverflow.com/questions/40800489/ffmpeg-read-frame-process-it-put-it-to-output-video-copy-sound-stream-unchan
	*/
	//int formatError = 0;
	//AVFormatContext * fmtContext = nullptr;
	//avformat_alloc_output_context2(&fmtContext, nullptr, nullptr, "testOut.mp4");
	//AVOutputFormat * outputFmt = fmtContext->oformat;
	//formatError = avio_open(&fmtContext->pb, "testOut.mp4", AVIO_FLAG_WRITE);
	//formatError = avformat_init_output(fmtContext, nullptr); /* reading docs, try this*/
	//formatError = avformat_write_header(fmtContext, nullptr);
	///* Guess you write packets here.. */
	//formatError = av_interleaved_write_frame(fmtContext, (AVPacket*)0);
	//formatError = av_write_trailer(fmtContext);
	//formatError = avio_closep(&fmtContext->pb);
	//avformat_free_context(fmtContext);

	/* Working: MP3 Encoding
	*/
	//int audioError = 0;
	//AVCodec* audioCodec = avcodec_find_encoder(AVCodecID::AV_CODEC_ID_MP3);
	//AVCodecContext* audioCodecContext = avcodec_alloc_context3(audioCodec);
	//audioCodecContext->bit_rate = 1280000;
	//audioCodecContext->sample_rate = 44100; // AudioIOMapper unified sample rate
	//audioCodecContext->channels = 2; // AudioIOMapper output channel count
	//audioCodecContext->channel_layout = AV_CH_LAYOUT_STEREO;
	//audioCodecContext->frame_size = 1024;

	//AVSampleFormat sf1 = audioCodec->sample_fmts[0];
	//AVSampleFormat sf2 = audioCodec->sample_fmts[1];
	//AVSampleFormat sf3 = audioCodec->sample_fmts[2];
	//AVSampleFormat sf4 = audioCodec->sample_fmts[3];

	//audioCodecContext->sample_fmt = AVSampleFormat::AV_SAMPLE_FMT_S32P; //audioCodec->sample_fmts[0]; // First allowed sample format (I think this is output side)
	//audioCodecContext->profile = FF_PROFILE_UNKNOWN;
	//audioCodecContext->codec_id = audioCodec->id;
	//audioCodecContext->codec_type = audioCodec->type;

	//audioError = avcodec_open2(audioCodecContext, audioCodec, nullptr);

	//std::ofstream testAudioStream{};
	//testAudioStream.open("testOutputAudio.mp3", std::ios::binary);

	//AVFrame * audioFrame = av_frame_alloc();
	//AVPacket * audioPacket = new AVPacket{};
	//for (int audioIter = 0; audioIter < 1000; ++audioIter)
	//{

	//	/* Set up sine wave */
	//	float * samples = new float[4096];
	//	for (int s = 0; s < (audioCodecContext->frame_size * audioCodecContext->channels); s += 2)
	//	{
	//		samples[s] = sinf((float)s / (10.0f + audioIter));
	//		samples[s + 1] = sinf((float)s / (15.0f));
	//	}
	//	//audioFrame->data[0] = samples;
	//	audioFrame->nb_samples = 1024;
	//	audioFrame->pts = audioIter * 1024;
	//	audioFrame->sample_rate = 96000;
	//	audioFrame->channels = 2;
	//	audioFrame->channel_layout = AV_CH_LAYOUT_STEREO;
	//	audioError = avcodec_fill_audio_frame(audioFrame, 2, AVSampleFormat::AV_SAMPLE_FMT_S32P, (uint8_t*)samples, 4096 * sizeof(float) / sizeof(uint8_t), 0);


	//	av_init_packet(audioPacket);
	//	audioPacket->data = nullptr;
	//	audioPacket->size = 0;

	//	audioError = avcodec_send_frame(audioCodecContext, audioFrame);
	//	audioError = avcodec_receive_packet(audioCodecContext, audioPacket);

	//	char * errorStr = new char[80];
	//	av_make_error_string(errorStr, 80, audioError);
	//	delete[] errorStr;

	//	if(audioError == 0)
	//		testAudioStream.write((char*)audioPacket->data, audioPacket->size);

	//	av_packet_unref(audioPacket);
	//}
	//// finish up
	//av_init_packet(audioPacket);
	//audioPacket->data = nullptr;
	//audioPacket->size = 0;
	//audioError = avcodec_send_frame(audioCodecContext, nullptr);
	//audioError = avcodec_receive_packet(audioCodecContext, audioPacket);
	//if (audioError == 0)
	//	testAudioStream.write((char*)audioPacket->data, audioPacket->size);
	//av_packet_unref(audioPacket);

	//testAudioStream.close();

	//delete audioPacket;
	//av_frame_free(&audioFrame);
	//avcodec_close(audioCodecContext);
	//av_free(audioCodecContext);
	///* /Audio Test Code */


	/* Handling failures */
	auto freeContext = [this]() {
		av_free(m_AVCodecContext);
		m_AVCodecContext = nullptr;
	};
	auto resetContext = [this, freeContext]() {
		avcodec_close(m_AVCodecContext);
		freeContext();
	};
	auto resetCtxFrameAndFileStream = [this, resetContext]() {
		resetContext();
		av_frame_free(&m_AVFrame);
		m_AVFrame = nullptr;
		m_fileStream.close();
	};
	auto resetAll = [this, resetCtxFrameAndFileStream] {
		av_freep(&m_AVFrame->data[0]); /* must free frame data before frame.. */
		m_AVFrame->data[0] = nullptr;
		resetCtxFrameAndFileStream();
	};

	if (!m_started)
	{
		int ret = 0;
		/* Bill: Apparently removing this is fine, but need to check. */
		//avcodec_register_all();
		m_codec = avcodec_find_encoder(codec_id);
		if (!m_codec) {
			fprintf(stderr, "Codec not found\n"); // No cleanup
			return StartResult::CodecNotFound;
		}
		m_AVCodecContext = avcodec_alloc_context3(m_codec);
		if (!m_AVCodecContext) {
			fprintf(stderr, "Could not allocate video codec context\n");
			return StartResult::ContextAllocationFailed; // No cleanup
		}
		m_AVCodecContext->bit_rate = 4000000;
		m_AVCodecContext->width = width;
		m_AVCodecContext->height = height;
		m_AVCodecContext->time_base.num = 1;
		m_AVCodecContext->time_base.den = fps;
		m_AVCodecContext->gop_size = 10;
		m_AVCodecContext->max_b_frames = 1;
		m_AVCodecContext->pix_fmt = AVPixelFormat::AV_PIX_FMT_YUV420P;

		if (codec_id == AV_CODEC_ID_H264)
			av_opt_set(m_AVCodecContext->priv_data, "preset", "slow", 0);
		if (avcodec_open2(m_AVCodecContext, m_codec, NULL) < 0) {
			fprintf(stderr, "Could not open codec\n");
			freeContext(); /* Free the unallocated codec context. */
			return StartResult::CodecUnopenable;
		}
		m_fileStream.open(filename, std::ios::binary);

		if (!m_fileStream.is_open()) {
			fprintf(stderr, "Could not open %s\n", filename);
			resetContext(); /* Reset the codec context. */
			return StartResult::FileUnopenable;
		}
		m_AVFrame = av_frame_alloc();
		if (!m_AVFrame) {
			fprintf(stderr, "Could not allocate video frame\n");
			resetCtxFrameAndFileStream(); /* Clear codec context, frame and close filestream. */
			return StartResult::AVFrameAllocationFailed;
		}
		m_AVFrame->format = m_AVCodecContext->pix_fmt;
		m_AVFrame->width = m_AVCodecContext->width;
		m_AVFrame->height = m_AVCodecContext->height;

		ret = av_image_alloc(m_AVFrame->data, m_AVFrame->linesize, m_AVCodecContext->width, m_AVCodecContext->height, m_AVCodecContext->pix_fmt, 32);
		if (ret < 0) {
			fprintf(stderr, "Could not allocate raw picture buffer\n");
			resetAll();
			return StartResult::PictureBufferAllocationFailed;
		}
		m_started = true;
		return StartResult::Success;
	}
	else
		return StartResult::EncoderAlreadyStarted;
}

void FFMPEG_Encoder::ffmpeg_encoder_render_frame()
{
	if (m_started)
	{
		m_AVFrame->pts = m_currentFrame;

		std::vector<uint8_t> rgb = readRGB();
		rgbIntoFrame(m_AVFrame, m_AVCodecContext, rgb);

		AVPacket packet;
		av_init_packet(&packet);
		frameToPacket(&packet, m_AVFrame, m_AVCodecContext);
		packetToFile(packet);
		av_packet_unref(&packet);

		/* We've rendered a frame. */
		++m_currentFrame;
		if (m_maxFrames != -1)
		{
			/* If we're done, finish: */
			if (m_maxFrames <= m_currentFrame)
				ffmpeg_encoder_finish();
		}
	}
}

FFMPEG_Encoder::FinishResult FFMPEG_Encoder::ffmpeg_encoder_finish()
{
	if (m_started)
	{
		uint8_t endcode[] = { 0, 0, 1, 0xb7 };
		int got_output = 0, ret = 0;
		do {
			//fflush(stdout);

			AVPacket packet;
			av_init_packet(&packet);

			ret = avcodec_send_frame(m_AVCodecContext, nullptr);
			ret = avcodec_receive_packet(m_AVCodecContext, &packet);
			if (ret < 0) {
				fprintf(stderr, "Error encoding frame\n");
				exit(1);
			}
			if (ret >= 0) {

				//fwrite(m_AVPacket.data, 1, m_AVPacket.size, m_file);
				/* BILL: TRY this, if it fails, you must check platform impls of char (they should be 8bits)..
							i.e. warning: you might need to test on both x86/x64 */
				m_fileStream.write((char*)packet.data, packet.size);
				av_packet_unref(&packet);
			}
		} while (got_output);
		m_fileStream.write((char*)endcode, sizeof(endcode));

		m_fileStream.close();

		avcodec_close(m_AVCodecContext);

		av_free(m_AVCodecContext);
		m_AVCodecContext = nullptr;
		av_freep(&m_AVFrame->data[0]);
		av_frame_free(&m_AVFrame);
		m_AVFrame = nullptr;

		m_started = false;
		return FinishResult::Success;
	}
	return FinishResult::EncoderNotStarted;
}

void FFMPEG_Encoder::frameToPacket(AVPacket * packet, AVFrame * frame, AVCodecContext * ctx)
{
	int ret = 0;
	packet->data = nullptr;
	packet->size = 0;
	ret = avcodec_send_frame(ctx, frame);
	ret = avcodec_receive_packet(ctx, packet);
	//return (ret >= 0);
	//char * errorStr = new char[80];
	//av_make_error_string(errorStr, 80, ret);
	//delete[] errorStr;
	//if (ret < 0) {
	//	fprintf(stderr, "Error encoding m_AVFrame\n");
	//}
	//else
	//{
	//m_fileStream.write((char*)m_AVPacket.data, m_AVPacket.size);
}

void FFMPEG_Encoder::packetToFile(const AVPacket & packet)
{
	m_fileStream.write((char*)packet.data, packet.size);
}
//return;


std::vector<uint8_t> FFMPEG_Encoder::readRGB()
{
	size_t i, j, k, cur_gl, cur_rgb, nvals;
	const size_t format_nchannels = m_pixelChannelCount = 4;

	nvals = format_nchannels * m_AVCodecContext->width * m_AVCodecContext->height;

	std::vector<GLubyte> pixelData;
	std::vector<uint8_t> rgb;
	pixelData.resize(nvals);
	rgb.resize(nvals);

	//*pixels = (GLubyte*)realloc(*pixels, nvals * sizeof(GLubyte));
	//*rgb = (uint8_t*)realloc(*rgb, nvals * sizeof(uint8_t));

	/* Get RGBA to align to 32 bits instead of just 24 for RGB. May be faster for FFmpeg. */
	glReadPixels(0, 0, m_AVCodecContext->width, m_AVCodecContext->height, GL_RGBA, GL_UNSIGNED_BYTE, &pixelData[0]);

	for (i = 0; i < m_AVCodecContext->height; i++) {
		for (j = 0; j < m_AVCodecContext->width; j++) {
			cur_gl = format_nchannels * (m_AVCodecContext->width * (m_AVCodecContext->height - i - 1) + j);
			cur_rgb = format_nchannels * (m_AVCodecContext->width * i + j);
			for (k = 0; k < format_nchannels; k++)
				rgb[cur_rgb + k] = pixelData[cur_gl + k];
		}
	}

	return rgb;
}
