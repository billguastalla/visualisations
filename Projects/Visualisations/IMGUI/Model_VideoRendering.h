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

	/* TODO: Connect this to the encoder and set it in the UI */
	int frameRate() { return m_frameRate; }
	bool setFrameRate(int fr);

	bool recordAudio() { return m_recordAudio; }
	void setRecordAudio(bool rec) { m_recordAudio = rec; }

	bool renderUI() { return m_renderUI; }
	void setRenderUI(bool ren) { m_renderUI = ren; }

	int frameCount() const { return m_frameCount; }


	const std::string framerateOptionsString() const;
	const int combo_FRtoOPT(int fr) const;
	const int combo_OPTToFR(int opt) const;

private:
	std::shared_ptr<Settings_VideoRendering> m_settings;
	std::shared_ptr<FFMPEG_Encoder> m_encoder;

	/* UI Values */
	RecordState m_recordState;
	std::string m_fileName;
	int m_frameRate;

	/*	This is a duplicate of (m_currentFrame in FFMPEG_Encoder) 
		only to allow the user to see the last video's total framecount after finishing rendering. */
	int m_frameCount;

	bool m_recordAudio;
	bool m_renderUI;
};