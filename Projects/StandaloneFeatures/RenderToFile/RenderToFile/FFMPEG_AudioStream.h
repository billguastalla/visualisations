#pragma once
#define __STDC_CONSTANT_MACROS
extern "C"
{
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

// a wrapper around a single output AVStream
class FFMPEG_AudioStream
{
public:
	AVStream* st; /* ALLOCATION: Allocated independently, but owned by an AVFormatContext (I think)
								Not explicitly de-allocated. */
	AVCodecContext* enc;/* ALLOCATION: Allocated independently  */

	/* pts of the next frame that will be generated */
	int64_t next_pts;
	int samples_count;

	AVFrame* frame; /* ALLOCATION: Allocated independently  */
	AVFrame* tmp_frame; /* ALLOCATION: Allocated independently  */

	float t, tincr, tincr2;

	struct SwrContext* swr_ctx; /* ALLOCATION: Allocated independently:
								Only for audio streams */


	AVFrame* get_audio_frame()
	{
		AVFrame* frame{ tmp_frame };
		int j{ 0 }, i{ 0 }, v{ 0 };
		int16_t* q = (int16_t*)frame->data[0];

		for (j = 0; j < frame->nb_samples; j++) {
			v = (int)(sin(t) * 10000);
			for (i = 0; i < enc->channels; i++)
				* q++ = v;
			t += tincr;
			tincr += tincr2;
		}
		frame->pts = next_pts;
		next_pts += frame->nb_samples;
		return frame;
	}
};
