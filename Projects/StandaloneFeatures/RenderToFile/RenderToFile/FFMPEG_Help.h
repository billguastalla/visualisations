#pragma once
extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat\avformat.h>
	#include <libavutil/imgutils.h>
	#include <libavutil/opt.h>
	#include <libswscale/swscale.h>
}

// From https://ffmpeg.org/doxygen/2.1/doc_2examples_2decoding_encoding_8c-example.html

/* check that a given sample format is supported by the encoder */
static int check_sample_fmt(AVCodec* codec, enum AVSampleFormat sample_fmt)
{
	const enum AVSampleFormat* p = codec->sample_fmts;
	while (*p != AV_SAMPLE_FMT_NONE) {
		if (*p == sample_fmt)
			return 1;
		p++;
	}
	return 0;
}
