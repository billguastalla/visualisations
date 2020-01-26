#pragma once
#include "Window_Abstract.h"
#include "Recorder.h"
#include <memory>

class Model_AudioInterface;

class Window_AudioInterface : public Window_Abstract
{
public:
	Window_AudioInterface(std::shared_ptr<Model_AudioInterface> & audioInterface);
	~Window_AudioInterface();

	void draw();
	const std::string windowTitle() const override { return "Audio Interface"; }
private:
	std::shared_ptr<Model_AudioInterface> m_audioInterface;

	std::shared_ptr<RecorderUI> m_recorderUI;

};
