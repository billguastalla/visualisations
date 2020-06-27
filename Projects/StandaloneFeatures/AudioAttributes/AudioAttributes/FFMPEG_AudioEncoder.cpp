#pragma once
#include "FFMPEG_AudioEncoder.h"
#include "AudioOutputMuxer.h"

#include <libavutil/error.h>
#include <iostream>
FFMPEG_AudioEncoder::FFMPEG_AudioEncoder()
	: m_started{ false }
{
}

static void encode(AVCodecContext* ctx, AVFrame* frame, AVPacket* pkt,
	std::ofstream & output)
{
	int ret;

	/* send the frame for encoding */
	ret = avcodec_send_frame(ctx, frame);
	if (ret < 0) {
		fprintf(stderr, "Error sending the frame to the encoder\n");
		exit(1);
	}

	/* read all the available output packets (in general there may be any
	 * number of them */
	while (ret >= 0) {
		ret = avcodec_receive_packet(ctx, pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			fprintf(stderr, "Error encoding audio frame\n");
			exit(1);
		}

		output.write((char*)pkt->data, pkt->size);
		av_packet_unref(pkt);
	}
}




FFMPEG_AudioEncoder::StartResult FFMPEG_AudioEncoder::ffmpeg_encoder_start(const char* filename, int sampleRate, int64_t bitRate)
{
	/* Working: MP3 Encoding */
	int audioError = 0;
	m_audioCodec = avcodec_find_encoder(AVCodecID::AV_CODEC_ID_MP3);
	m_audioCodecContext = avcodec_alloc_context3(m_audioCodec);
	m_audioCodecContext->bit_rate = bitRate;
	m_audioCodecContext->sample_rate = sampleRate; // AudioIOMapper unified sample rate
	m_audioCodecContext->channels = 2; // AudioIOMapper output channel count
	m_audioCodecContext->channel_layout = AV_CH_LAYOUT_STEREO;
	//m_audioCodecContext->frame_size = 1024;

	AVSampleFormat sf1 = m_audioCodec->sample_fmts[0];
	AVSampleFormat sf2 = m_audioCodec->sample_fmts[1];
	AVSampleFormat sf3 = m_audioCodec->sample_fmts[2];
	AVSampleFormat sf4 = m_audioCodec->sample_fmts[3];

	m_audioCodecContext->sample_fmt = AVSampleFormat::AV_SAMPLE_FMT_FLTP; //audioCodec->sample_fmts[0]; // First allowed sample format (I think this is output side)
	m_audioCodecContext->profile = FF_PROFILE_UNKNOWN;
	m_audioCodecContext->codec_id = m_audioCodec->id;
	m_audioCodecContext->codec_type = m_audioCodec->type;

	audioError = avcodec_open2(m_audioCodecContext, m_audioCodec, nullptr);

	m_audioFileStream.open(filename, std::ios::binary);

	m_audioFrame = av_frame_alloc();


	m_audioFrame->nb_samples = m_audioCodecContext->frame_size;
	m_audioFrame->channels = m_audioCodecContext->channels;
	m_audioFrame->channel_layout = m_audioCodecContext->channel_layout;
	int ret = av_frame_get_buffer(m_audioFrame, 0);

	/* Handling failures */
	//auto freeContext = [this]() {
	//	av_free(m_AVCodecContext);
	//	m_AVCodecContext = nullptr;
	//};
	//auto resetContext = [this, freeContext]() {
	//	avcodec_close(m_AVCodecContext);
	//	freeContext();
	//};
	//auto resetCtxFrameAndFileStream = [this, resetContext]() {
	//	resetContext();
	//	av_frame_free(&m_AVFrame);
	//	m_AVFrame = nullptr;
	//	m_fileStream.close();
	//};
	//auto resetAll = [this, resetCtxFrameAndFileStream] {
	//	av_freep(&m_AVFrame->data[0]); /* must free frame data before frame.. */
	//	m_AVFrame->data[0] = nullptr;
	//	resetCtxFrameAndFileStream();
	//};

	m_started = true;
	return StartResult::Success;
}

int FFMPEG_AudioEncoder::ffmpeg_encoder_render_frame(AudioIO::AudioOutputMuxer& muxer, int startingSample)
{
	if (m_started)
	{
		//std::vector<float> samples{ muxer.samples(startingSample,m_audioCodecContext->frame_size) };

		std::vector<float> samples;
		samples.clear();
		for (int i = startingSample; i < startingSample + m_audioCodecContext->frame_size; ++i)
		{
			samples.push_back(sin(2.f * 3.14159f * (float)i * 4400 / (float)44100));
			samples.push_back(sin(2.f * 3.14159f * (float)i * 440 / (float)44100));
		}

		m_audioFrame->nb_samples = m_audioCodecContext->frame_size;
		m_audioFrame->sample_rate = m_audioCodecContext->sample_rate;
		m_audioFrame->channels = m_audioCodecContext->channels;
		m_audioFrame->channel_layout = m_audioCodecContext->channel_layout;
		m_audioFrame->pts = startingSample;



		int s1 = m_audioCodec->supported_samplerates[0];
		int s2 = m_audioCodec->supported_samplerates[1];
		int s3 = m_audioCodec->supported_samplerates[2];


		int ret = av_frame_make_writable(m_audioFrame);

		int bufSize = (int)samples.size() * sizeof(float) / sizeof(uint8_t);
		int audioError = avcodec_fill_audio_frame(m_audioFrame, 2, AVSampleFormat::AV_SAMPLE_FMT_FLTP, (uint8_t*)&samples[0], bufSize, 0);







		av_init_packet(&m_audioPacket);
		m_audioPacket.data = nullptr;
		m_audioPacket.size = 0;



		//audioError = avcodec_send_frame(m_audioCodecContext, m_audioFrame);

		//while (audioError >= 0)
		//{
		//	audioError = avcodec_receive_packet(m_audioCodecContext, &m_audioPacket);
		//	if (audioError == AVERROR(EAGAIN) || audioError == AVERROR_EOF)
		//		return startingSample + m_audioFrame->nb_samples;
		//	else if (audioError < 0) {
		//		fprintf(stderr, "Error encoding audio frame\n");
		//		exit(1);
		//	}

		//	std::cout << "Packet: {PTS:" << m_audioPacket.pts << ", SIZE:" << m_audioPacket.size << ", DURATION:" << m_audioPacket.duration << "}\n";

		//	m_audioFileStream.write((char*)m_audioPacket.data, m_audioPacket.size);

		//}
		//av_packet_unref(&m_audioPacket);

		encode(m_audioCodecContext, m_audioFrame, &m_audioPacket,m_audioFileStream);

		char* errorStr = new char[80];
		av_make_error_string(errorStr, 80, audioError);
		delete[] errorStr;

		//if (audioError == 0)
		//{
		//	m_audioFileStream.write((char*)m_audioPacket.data, m_audioPacket.size);
		//}
		//av_packet_unref(&m_audioPacket);

		return startingSample + m_audioFrame->nb_samples;
	}
	return startingSample;
}

FFMPEG_AudioEncoder::FinishResult FFMPEG_AudioEncoder::ffmpeg_encoder_finish()
{
	// finish up
	//av_init_packet(&m_audioPacket);
	//m_audioPacket.data = nullptr;
	//m_audioPacket.size = 0;
	//int audioError = avcodec_send_frame(m_audioCodecContext, nullptr);
	//audioError = avcodec_receive_packet(m_audioCodecContext, &m_audioPacket);
	//if (audioError == 0)
	//	m_audioFileStream.write((char*)m_audioPacket.data, m_audioPacket.size);
	//av_packet_unref(&m_audioPacket);

	encode(m_audioCodecContext, nullptr, &m_audioPacket, m_audioFileStream);

	m_audioFileStream.close();

	//delete m_audioPacket;
	av_frame_free(&m_audioFrame);
	avcodec_close(m_audioCodecContext);
	av_free(m_audioCodecContext);
	///* /Audio Test Code */

	m_started = false;
	return FinishResult::Success;

}
