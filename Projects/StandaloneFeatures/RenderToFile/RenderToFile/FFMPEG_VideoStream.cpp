#include "FFMPEG_VideoStream.h"

#define SCALE_FLAGS SWS_BICUBIC

void FFMPEG_VideoStream::open(AVFormatContext* oc, AVDictionary* opt_arg)
{
	int ret{ 0 };
	AVCodecContext* c{ m_avcodecEncoderContext };
	AVDictionary* opt{ nullptr };

	av_dict_copy(&opt, opt_arg, 0);

	/* open the codec */
	ret = avcodec_open2(c, p_codec, &opt);
	av_dict_free(&opt);
	if (ret < 0)
	{
		reportError("Opening Video Stream: Could not open video codec. " \
			"Possible Causes: Video parameters incompatible with codec.", ret);
	}

	/* allocate and init a re-usable frame */
	m_streamFrame = alloc_picture(c->pix_fmt, c->width, c->height);
	if (!m_streamFrame) {
		reportError("Opening Video Stream: Could not allocate video frame.", ret);
	}

	/* If the output format is not YUV420P, then a temporary YUV420P
	 * picture is needed too. It is then converted to the required
	 * output format. */
	m_streamFrameTemp = nullptr;
	if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
		m_streamFrameTemp = alloc_picture(AV_PIX_FMT_YUV420P, c->width, c->height);
		if (!m_streamFrameTemp) {
			reportError("Opening Video Stream: Could not allocate temporary picture.");
		}
	}

	/* copy the stream parameters to the muxer */
	ret = avcodec_parameters_from_context(m_avstream->codecpar, c);
	if (ret < 0) {
		reportError("Opening Video Stream: Could not copy the stream parameters.", ret);
	}
}


bool FFMPEG_VideoStream::buildPacket(AVPacket& packet, AVFormatContext* oc)
{
	int ret{ 0 };
	int got_packet = 0;
	AVCodecContext* c{ m_avcodecEncoderContext };
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
		frame = getFrame();
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
		//ret = write_frame(oc, &c->time_base, m_avstream, &pkt);
	}
	if (ret < 0) {
		reportError("Writing Video Frame: Error while writing video frame.", ret);
	}
	return (frame || got_packet);
}

AVFrame* FFMPEG_VideoStream::getFrame()
{
	AVCodecContext* c{ m_avcodecEncoderContext };

	/* when we pass a frame to the encoder, it may keep a reference to it
	 * internally; make sure we do not overwrite it here */
	if (int r = av_frame_make_writable(m_streamFrame) < 0)
		reportError("Getting Video Frame: Could not make frame writable.", r);

	if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
		/* as we only generate a YUV420P picture, we must convert it
		 * to the codec pixel format if needed */
		if (!sws_ctx) {
			sws_ctx = sws_getContext(c->width, c->height,
				AV_PIX_FMT_YUV420P,
				c->width, c->height,
				c->pix_fmt,
				SCALE_FLAGS, nullptr, nullptr, nullptr);
			if (!sws_ctx) {
				reportError("Getting Video Frame: Could not initialize the conversion context.");
			}
		}
		fill_yuv_image(
			m_streamFrameTemp,
			(int)m_nextFramePTS,
			c->width,
			c->height);
		sws_scale(sws_ctx, (const uint8_t * const*)m_streamFrameTemp->data,
			m_streamFrameTemp->linesize, 0, c->height, m_streamFrame->data,
			m_streamFrame->linesize);
	}
	else {
		fill_yuv_image(m_streamFrame, (int)m_nextFramePTS, c->width, c->height);
	}

	m_streamFrame->pts = m_nextFramePTS++;

	return m_streamFrame;
}

void FFMPEG_VideoStream::close()
{
	avcodec_free_context(&m_avcodecEncoderContext);
	av_frame_free(&m_streamFrame);
	av_frame_free(&m_streamFrameTemp);
	sws_freeContext(sws_ctx);
	//swr_free(&swr_ctx);
	m_initialised = false;
}

void FFMPEG_VideoStream::fill_yuv_image(AVFrame* pict, int frame_index, int width, int height)
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

AVFrame* FFMPEG_VideoStream::alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
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