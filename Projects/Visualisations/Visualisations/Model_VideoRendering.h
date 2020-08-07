#pragma once
#include <memory>
#include <string>
#include <boost/property_tree/ptree_fwd.hpp>

class Settings_VideoRendering;
class FFMPEG_Encoder;
class Program;

struct GLFWwindow;


/* Not used yet: working out what's important about file rendering. */
struct RenderSettings
{
	RenderSettings():
		width{ 1920 },
		height{ 1080 },
		fps{ 30 },
		bitRate{ 4000000 },
		filename{""}
	{}
	int width, height;
	int fps;
	int bitRate;
	std::string filename;
};

class Model_VideoRendering
{

public:
	bool loadFileTree(const boost::property_tree::ptree& t);
	bool saveFileTree(boost::property_tree::ptree& t) const;

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
	Model_VideoRendering(Program * p_program, GLFWwindow * window);
	~Model_VideoRendering();

	void renderFrame();

	void takePicture(const std::string & filename);

	RecordState state() { return m_recordState; }
	bool start();
	bool pause();
	bool stop();

	std::string fileName() const { return m_fileName; }
	void setFileName(std::string fn) { m_fileName = fn; }

	/* TODO: Connect this to the encoder and set it in the UI */
	int frameRate() const { return m_frameRate; }
	bool setFrameRate(int fr);

	bool recordAudio() const { return m_recordAudio; }
	void setRecordAudio(bool rec) { m_recordAudio = rec; }

	bool renderUI() const { return m_renderUI; }
	void setRenderUI(bool ren) { m_renderUI = ren; }

	bool renderFromMainBuffer() const { return m_renderFromMainBuffer; }
	void setrenderFromMainBuffer(bool ren) { m_renderFromMainBuffer = ren; }

	int frameCount() const { return m_frameCount; }


	const std::string framerateOptionsString() const;
	const int combo_FRtoOPT(int fr) const;
	const int combo_OPTToFR(int opt) const;

private:

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
	bool m_renderFromMainBuffer;

	GLFWwindow* m_window;
	Program* p_program;
};
