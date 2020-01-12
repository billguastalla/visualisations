#include "FFMPEG_AudioStream.h"
#include "FFMPEG_Muxing.h"
void FFMPEG_AudioStream::setCodecContextParameters(const MuxerSettings & settings)
{
	m_avcodecEncoderContext->sample_fmt = p_codec->sample_fmts ? p_codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
	m_avcodecEncoderContext->bit_rate = settings.m_audioBitRate;
	m_avcodecEncoderContext->sample_rate = settings.m_audioDefaultSampleRate;
	if (p_codec->supported_samplerates) {
		m_avcodecEncoderContext->sample_rate = p_codec->supported_samplerates[0];
		for (int i = 0; p_codec->supported_samplerates[i]; i++) {
			if (p_codec->supported_samplerates[i] == 44100)
				m_avcodecEncoderContext->sample_rate = 44100;
		}
	}
	m_avcodecEncoderContext->channels = av_get_channel_layout_nb_channels(m_avcodecEncoderContext->channel_layout);
	m_avcodecEncoderContext->channel_layout = AV_CH_LAYOUT_STEREO;
	if (p_codec->channel_layouts) {
		m_avcodecEncoderContext->channel_layout = p_codec->channel_layouts[0];
		for (int i = 0; p_codec->channel_layouts[i]; i++) {
			if (p_codec->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
				m_avcodecEncoderContext->channel_layout = AV_CH_LAYOUT_STEREO;
		}
	}
	m_avcodecEncoderContext->channels = av_get_channel_layout_nb_channels(m_avcodecEncoderContext->channel_layout);
	/* B.G: Non-standard type-conversion. */
	// AVRational = { .num = 1,.den = m_avcodecEncoderContext->sample_rate }; (one day we can use designated initialisers, C++20)
	m_avstream->time_base = av_make_q(1, m_avcodecEncoderContext->sample_rate);
}

bool FFMPEG_AudioStream::buildPacket(AVPacket & packet, AVFormatContext* oc)
{
	AVCodecContext * c{ m_avcodecEncoderContext };
	//AVPacket pkt = { 0 }; // data and size must be 0;
	AVFrame* frame{ nullptr };
	int ret{ 0 };
	int got_packet{ 0 };
	int dst_nb_samples{ 0 };

	/* B.G TODO: Consider moving outside. */
	av_init_packet(&packet);

	/* B.G ISSUE: The pts of the next frame is updated without knowing if it was muxed in. */
	frame = getFrame();

	if (frame) {
		/* convert samples from native format to destination codec format, using the resampler */
			/* compute destination number of samples */
		dst_nb_samples = (int)av_rescale_rnd(
			swr_get_delay(swr_ctx, c->sample_rate) + (int64_t)frame->nb_samples,
			(int64_t)c->sample_rate,
			(int64_t)c->sample_rate,
			AV_ROUND_UP);
		av_assert0(dst_nb_samples == frame->nb_samples);

		/* when we pass a frame to the encoder, it may keep a reference to it
		 * internally;
		 * make sure we do not overwrite it here
		 */
		ret = av_frame_make_writable(m_streamFrame);
		if (ret < 0)
		{
			reportError("Writing Audio Frame: Could not make frame writable", ret);
		}

		/* convert to destination format */
		ret = swr_convert(swr_ctx,
			m_streamFrame->data, dst_nb_samples,
			(const uint8_t * *)frame->data, frame->nb_samples);
		if (ret < 0) {
			reportError("Writing Audio Frame: Error while converting frame \n", ret);
		}
		frame = m_streamFrame;

		/* B.G: Inserted av_make_q().  */
		frame->pts = av_rescale_q(samples_count, av_make_q(1, c->sample_rate), c->time_base);
		samples_count += dst_nb_samples;
	}

	//ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
	/* B.G avcodec_encode_audio2 is deprecated! */
	if (frame != nullptr)
	{
		ret = avcodec_send_frame(c, frame);
		got_packet = !avcodec_receive_packet(c, &packet);
	}
	else
	{
		/* No frame to encode. */
		return 1;
	}

	if (ret < 0)
	{
		reportError("Writing Audio Frame: Error encoding audio frame: ", ret);
	}

	if (got_packet) {
		//ret = write_frame(oc, &c->time_base, m_avstream, &packet);
		if (ret < 0) {
			reportError("Writing Audio Frame: Error while writing audio frame: ", ret);
		}
	}

	return (got_packet);
}

AVFrame* FFMPEG_AudioStream::getFrame()
{
	//AVFrame* frame{ m_streamFrameTemp };
	int j{ 0 }, i{ 0 }, v{ 0 };
	int16_t* q = (int16_t*)m_streamFrameTemp->data[0];

	for (j = 0; j < m_streamFrameTemp->nb_samples; j++) {
		v = (int)(sin(t) * 10000);
		for (i = 0; i < m_avcodecEncoderContext->channels; i++)
			* q++ = v;
		t += tincr;
		tincr += tincr2;
	}
	m_streamFrameTemp->pts = m_nextFramePTS;
	m_nextFramePTS += m_streamFrameTemp->nb_samples;
	return m_streamFrameTemp;
}

void FFMPEG_AudioStream::open(AVFormatContext* oc, AVDictionary* opt_arg)
{
	int nb_samples{ 0 };
	int ret{ 0 };
	AVDictionary* opt{ nullptr };
	AVCodecContext* c{ m_avcodecEncoderContext };

	/* open it */
	av_dict_copy(&opt, opt_arg, 0);
	ret = avcodec_open2(c, p_codec, &opt);
	av_dict_free(&opt);
	if (ret < 0) {
		/* B.G: NOTE: This can happen if an unsupported sample rate is provided,
						when the supported sample rates are not shown for the codec. */
		reportError("Could not open audio codec.", ret);
	}

	/* init signal generator */
	t = 0;
	tincr = (float)(2.0 * M_PI * 110.0 / c->sample_rate);
	/* increment frequency by 110 Hz per second */
	tincr2 = (float)(2.0 * M_PI * 110.0 / c->sample_rate / c->sample_rate);

	/* B.G: If we can send large frames then do it. */
	if (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
		nb_samples = 10000;
	else
		nb_samples = c->frame_size;

	/* B.G: QUESTION: Why are the sample formats different? */
	m_streamFrame = alloc_audio_frame(c->sample_fmt, c->channel_layout,
		c->sample_rate, nb_samples);
	m_streamFrameTemp = alloc_audio_frame(AV_SAMPLE_FMT_S16, c->channel_layout,
		c->sample_rate, nb_samples);

	/* copy the stream parameters to the muxer */
	ret = avcodec_parameters_from_context(m_avstream->codecpar, c);
	if (ret < 0) {
		reportError("Could not copy the stream parameters\n", ret);
	}

	/* create resampler context */
	swr_ctx = swr_alloc();
	if (!swr_ctx) {
		reportError("Could not allocate the resampler context\n");
	}

	/* set options */
	av_opt_set_int(swr_ctx, "in_channel_count", c->channels, 0);
	av_opt_set_int(swr_ctx, "in_sample_rate", c->sample_rate, 0);
	av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
	av_opt_set_int(swr_ctx, "out_channel_count", c->channels, 0);
	av_opt_set_int(swr_ctx, "out_sample_rate", c->sample_rate, 0);
	av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", c->sample_fmt, 0);

	/* initialize the resampling context */
	if ((ret = swr_init(swr_ctx)) < 0) {
		reportError("Failed to initialize the resampling context\n", ret);
	}
}

void FFMPEG_AudioStream::close()
{
	avcodec_free_context(&m_avcodecEncoderContext);
	av_frame_free(&m_streamFrame);
	av_frame_free(&m_streamFrameTemp);
	//sws_freeContext(sws_ctx);
	swr_free(&swr_ctx);
	m_initialised = false;
}

AVFrame* FFMPEG_AudioStream::alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout,
	int sample_rate, int nb_samples)
{
	AVFrame* frame{ av_frame_alloc() };
	int ret{ 0 };

	if (!frame) {
		reportError("Error allocating an audio frame.");
		return nullptr;
	}

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
