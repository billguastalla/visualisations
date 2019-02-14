#pragma once
#include <memory>

#include <string>

class Settings_VideoRendering;
class FFMPEG_Encoder;

class Model_VideoRendering
{

public:
	enum class RecordState
	{
		Started,
		Paused,
		Stopped
	};

	/*
		-> Some things the Model needs to do:
			-> Start/stop recording and keep track of this:
				-> Prevent various settings being modified while recording
			->
			-> Handle a separate framebuffer, and handle *what it is that the framebuffer draws*.
				-> (UI enabled/disabled option)
			-> Handle the width/height of the framebuffer, and permit/forbid modification of
				width & height during recording
	*/
	Model_VideoRendering(std::shared_ptr<Settings_VideoRendering> & settings);
	~Model_VideoRendering();

	void renderFrame();

	RecordState state() { return m_recordState; }
	bool start();
	bool pause();
	bool stop();

	std::string fileName() { return m_fileName; }
	void setFileName(std::string fn) { m_fileName = fn; }

	int frameRate() { return m_frameRate; }
	void setFrameRate(int fr) { m_frameRate = fr; }

	bool recordAudio() { return m_recordAudio; }
	void setRecordAudio(bool rec) { m_recordAudio = rec; }

	bool renderUI() { return m_renderUI; }
	void setRenderUI(bool ren) { m_renderUI = ren; }
private:
	std::shared_ptr<Settings_VideoRendering> m_settings;
	std::shared_ptr<FFMPEG_Encoder> m_encoder;

	/* UI Values */
	//const char *filename, AVCodecID codec_id, int fps, int width, int height
	RecordState m_recordState;
	std::string m_fileName;
	int m_frameRate;
	bool m_recordAudio;
	bool m_renderUI;
};
