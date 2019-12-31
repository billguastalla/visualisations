#pragma once
#include "FFMPEG_AudioEncoder.h"
#include "AudioOutputMuxer.h"

FFMPEG_AudioEncoder::FFMPEG_AudioEncoder()
	: m_started{ false }
{
}

FFMPEG_AudioEncoder::StartResult FFMPEG_AudioEncoder::ffmpeg_encoder_start(const char *filename, int sampleRate, int64_t bitRate)
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

int FFMPEG_AudioEncoder::ffmpeg_encoder_render_frame(AudioIO::AudioOutputMuxer & muxer, int startingSample)
{
	if (m_started)
	{
		std::vector<float> samples{ muxer.samples(startingSample,m_audioCodecContext->frame_size) };
		m_audioFrame->nb_samples = m_audioCodecContext->frame_size;
		m_audioFrame->pts = startingSample;
		m_audioFrame->sample_rate = 48000;
		m_audioFrame->channels = 2;
		m_audioFrame->channel_layout = AV_CH_LAYOUT_STEREO;

		int audioError = avcodec_fill_audio_frame(m_audioFrame, 2, AVSampleFormat::AV_SAMPLE_FMT_FLTP, (uint8_t*)&samples[0], (int)samples.size() * sizeof(float) / sizeof(uint8_t), 0);

		av_init_packet(&m_audioPacket);
		m_audioPacket.data = nullptr;
		m_audioPacket.size = 0;

		audioError = avcodec_send_frame(m_audioCodecContext, m_audioFrame);
		audioError = avcodec_receive_packet(m_audioCodecContext, &m_audioPacket);

		int retry{ 0 };
		while (audioError < 0 && (retry < 100))
		{
			audioError = avcodec_receive_packet(m_audioCodecContext, &m_audioPacket);
			++retry;
		}

		char * errorStr = new char[80];
		av_make_error_string(errorStr, 80, audioError);
		delete[] errorStr;

		if (audioError == 0)
		{
			m_audioFileStream.write((char*)m_audioPacket.data, m_audioPacket.size);
		}

		av_packet_unref(&m_audioPacket);

		return m_audioCodecContext->frame_size + startingSample;
	}
	return startingSample;
}

FFMPEG_AudioEncoder::FinishResult FFMPEG_AudioEncoder::ffmpeg_encoder_finish()
{
	// finish up
	av_init_packet(&m_audioPacket);
	m_audioPacket.data = nullptr;
	m_audioPacket.size = 0;
	int audioError = avcodec_send_frame(m_audioCodecContext, nullptr);
	audioError = avcodec_receive_packet(m_audioCodecContext, &m_audioPacket);
	if (audioError == 0)
		m_audioFileStream.write((char*)m_audioPacket.data, m_audioPacket.size);
	av_packet_unref(&m_audioPacket);

	m_audioFileStream.close();

	//delete m_audioPacket;
	av_frame_free(&m_audioFrame);
	avcodec_close(m_audioCodecContext);
	av_free(m_audioCodecContext);
	///* /Audio Test Code */

	m_started = false;
	return FinishResult::Success;

}
