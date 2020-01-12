#pragma once
#include <glad/glad.h>
#include <GLFW\glfw3.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat\avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

#include <fstream>
#include <vector>

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
		AVFrameEncodingFailed,
		EncoderNotStarted
	};

	/* Start: run once before render loop, after building scene */
	StartResult ffmpeg_encoder_start(const char *filename, AVCodecID codec_id, int fps, int width, int height);
	/* */
	void ffmpeg_encoder_render_frame();
	/* Finish: */
	FinishResult ffmpeg_encoder_finish();

	int currentFrame() { return m_currentFrame; }
private:
	bool m_started;

	/*	
		- Read RGB
		- Put into Frame
		- Put into Packet
		- Write Packet
	*/

	/* Read rgb data into vector from gl() */
	std::vector<uint8_t> readRGB();
	/* Insert rgb data into an AVFrame */
	void rgbIntoFrame(AVFrame * frame, const AVCodecContext * ctx, const std::vector<uint8_t> & rgbData);
	/* Caller is responsible for init/deinit of packet. */
	void frameToPacket(AVPacket * packetOut, AVFrame * frameIn, AVCodecContext * ctx);

	void packetToFile(const AVPacket & packet);

	/* This is new, but might be useful. */
	AVCodec * m_codec;

	/* Originals */
	//FILE * m_file;
	std::ofstream m_fileStream;

	AVCodecContext * m_AVCodecContext = NULL;
	AVFrame * m_AVFrame;
	AVPacket m_AVPacket;
	SwsContext * m_swsContext = NULL;


	//std::vector<uint8_t> m_rgbData;
	int m_pixelChannelCount;
	
	/* uint8_t * m_rgb = NULL;
	GLubyte * m_pixels = NULL; */

	/* Frame counting */
	bool m_countFrames = true;
	int m_maxFrames = -1;
	int m_currentFrame;
};