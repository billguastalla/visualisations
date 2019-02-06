#pragma once
#include "Window_Abstract.h"
#include <memory>

class Controller_VideoRendering;

class Window_VideoRendering : public Window_Abstract
{
public:
	Window_VideoRendering(std::shared_ptr<Controller_VideoRendering> & controller);
	~Window_VideoRendering();

	void draw();
private:
	std::shared_ptr<Controller_VideoRendering> m_videoController;
};

