#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

#include <fstream>
#include <vector>

/* Class for standalone encoding of audio only.
	TODO: Build an encoder for audio *and* video
*/
namespace AudioIO
{
	class AudioOutputMuxer;
}

class FFMPEG_AudioEncoder
{
public:
	FFMPEG_AudioEncoder();

	enum class StartResult
	{
		Success,
		CodecNotFound,
		ContextAllocationFailed,
		CodecUnopenable,
		FileUnopenable,
		AVFrameAllocationFailed,
		PictureBufferAllocationFailed,
		EncoderAlreadyStarted
	};
	enum class FinishResult
	{
		Success,
		AVFrameEncodingFailed,
		EncoderNotStarted
	};
	/* Start mp3 */
	StartResult ffmpeg_encoder_start(const char *filename, int sampleRate = 48000, int64_t bitRate = 1280000);
	/* TODO: Think more carefully about:
				-> Arguments,
				-> Sample pos etc
	*/
	int ffmpeg_encoder_render_frame(AudioIO::AudioOutputMuxer & muxer, int startingSample);
	/* Finish mp3: */
	FinishResult ffmpeg_encoder_finish();

private:
	bool m_started;
	AVCodec * m_audioCodec;
	std::ofstream m_audioFileStream;
	AVCodecContext * m_audioCodecContext;
	AVFrame * m_audioFrame;
	AVPacket m_audioPacket;

};