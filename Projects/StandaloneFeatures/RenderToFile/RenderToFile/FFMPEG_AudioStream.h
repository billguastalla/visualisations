#pragma once
#include "FFMPEG_Stream.h"

class FFMPEG_AudioStream : public FFMPEG_Stream
{
public:
	FFMPEG_AudioStream() :
		FFMPEG_Stream{},
		t{ 0 },
		tincr{ 0 },
		tincr2{ 0 },
		swr_ctx{nullptr},
		m_sampleCount{ 0 }
	{}

	bool buildPacket(AVPacket& packet, AVFormatContext* oc) override;

	void open(AVFormatContext* oc, AVDictionary* opt_arg) override;
	void close() override;
protected:
	AVFrame* getFrame() override;
	void setCodecContextParameters(const MuxerSettings & settings) override;

private:
	AVFrame* alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples);

private:
	int m_sampleCount;
	struct SwrContext* swr_ctx;
	float t;
	float tincr;
	float tincr2;
};