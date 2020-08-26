#pragma once
#include "Window_Abstract.h"
#include "Recorder.h"
#include "UI_Recorder.h"
#include <memory>

class Model_AudioInterface;

class Window_AVIO : public Window_Abstract
{
public:
	Window_AVIO(std::shared_ptr<Model_AudioInterface> & audioInterface);
	~Window_AVIO();

	void draw();
	const std::string windowTitle() const override { return "Audio / Video"; }
private:


	std::shared_ptr<Model_AudioInterface> m_audioInterface;
	std::shared_ptr<RecorderUI> m_recorderUI;
};
