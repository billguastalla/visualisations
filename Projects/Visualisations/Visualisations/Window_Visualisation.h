#pragma once
#include "Window_Abstract.h"
#include <memory>

class Model_Visualisation;

class Window_Visualisation : public Window_Abstract
{
public:
	Window_Visualisation(std::shared_ptr<Model_Visualisation> & visualisation);
	~Window_Visualisation();

	void draw();
	const std::string windowTitle() const override { return "Visualisation Settings"; }


private:
	std::shared_ptr<Model_Visualisation> m_visualisation;
};

