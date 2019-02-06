#pragma once
#include "Window_Abstract.h"

#include <memory>

class Controller_AudioInterface;

class Window_AudioInterface : public Window_Abstract
{
public:
	Window_AudioInterface(std::shared_ptr<Controller_AudioInterface> & audioInterface);
	~Window_AudioInterface();

	void draw();
private:
	std::shared_ptr<Controller_AudioInterface> m_audioInterface;
};

