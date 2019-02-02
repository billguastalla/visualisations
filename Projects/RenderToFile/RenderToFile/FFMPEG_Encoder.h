#pragma once
#include <GLFW\glfw3.h>

#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>

/* Adapted from: https://github.com/cirosantilli/cpp-cheat/blob/19044698f91fefa9cb75328c44f7a487d336b541/ffmpeg/encode.c */

/*
TODO:
	-> Create start/finish state and prevent rendering without starting, stopping without starting etc.
	-> Store all constructor properties in one structure (Filename, Codec, FPS, width, height)
	-> Constructor should allocate memory appropriately for all members
	-> Destructor should delete appropriately.
	-> All "Exits" should be substituted: stop the stream & reset variables.
*/
class FFMPEG_Encoder
{
public:
	FFMPEG_Encoder();

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
		AVFrameEncodingFailed
	};

	/* Start: run once before render loop, after building scene */
	StartResult ffmpeg_encoder_start(const char *filename, AVCodecID codec_id, int fps, int width, int height);
	/* */
	void ffmpeg_encoder_render_frame();
	/* Finish: */
	FinishResult ffmpeg_encoder_finish();

private:
	void allocate();
	void unallocate();

	bool m_started;

	void ffmpeg_encoder_glread_rgb(uint8_t **rgb, GLubyte **pixels, unsigned int width, unsigned int height);
	void ffmpeg_encoder_set_frame_yuv_from_rgb(uint8_t *rgb);
	void ffmpeg_encoder_encode_frame(uint8_t *rgb);

	/* This is new, but might be useful. */
	AVCodec * m_codec;

	/* Originals */
	AVCodecContext * m_AVCodecContext = NULL;
	AVFrame * m_AVFrame;
	AVPacket m_AVPacket;
	FILE * m_file;
	SwsContext * m_swsContext = NULL;
	uint8_t * m_rgb = NULL;
	GLubyte * m_pixels = NULL;

	/* Frame counting */
	bool m_countFrames = true;
	int m_maxFrames = -1;
	int m_frameCount = 0;
};