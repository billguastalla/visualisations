#pragma once
#include "Window_Abstract.h"
#include <memory>

class Model_ViewportSystem;

class Window_ViewportSystem : public Window_Abstract
{
public:
	Window_ViewportSystem(std::shared_ptr<Model_ViewportSystem>& viewport);
	~Window_ViewportSystem();

	void draw();
	const std::string windowTitle() const override { return "Viewport"; }
private:
	std::shared_ptr<Model_ViewportSystem> m_viewportSystem;
};
