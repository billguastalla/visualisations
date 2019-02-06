#pragma once
#include "Window_Abstract.h"
#include <memory>

class Controller_Visualisation;

class Window_Visualisation : public Window_Abstract
{
public:
	Window_Visualisation(std::shared_ptr<Controller_Visualisation> & visualisation);
	~Window_Visualisation();

	void draw();
private:
	std::shared_ptr<Controller_Visualisation> m_visualisation;
};

