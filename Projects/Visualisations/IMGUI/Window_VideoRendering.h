#pragma once
#include "Window_Abstract.h"
#include <memory>

class Model_VideoRendering;

class Window_VideoRendering : public Window_Abstract
{
public:
	Window_VideoRendering(std::shared_ptr<Model_VideoRendering> & Model);
	~Window_VideoRendering();

	void draw();
	const std::string windowTitle() const override { return "Video Rendering"; }
	const std::string recordState() const;
private:
	std::shared_ptr<Model_VideoRendering> m_videoModel;
};

