#pragma once
#include "Window_Abstract.h"

#include <memory>
#include <array>

class PostProcessing;

class Window_PostProcessing : public Window_Abstract
{
public:
	Window_PostProcessing(std::shared_ptr<PostProcessing> & pprocessing);
	~Window_PostProcessing();

	void draw();
	const std::string windowTitle() const override { return "Post Processing"; }
private:
	std::shared_ptr<PostProcessing> p_postProcessing;
	std::array<int, 2> ui_mainFramebufferResolution;
};
#pragma once
