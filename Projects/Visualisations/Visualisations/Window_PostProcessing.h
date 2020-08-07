#pragma once
#include "Window_Abstract.h"
#include "Recorder.h"
#include <memory>

class PostProcessing;

class Window_PostProcessing : public Window_Abstract
{
public:
	Window_PostProcessing(std::shared_ptr<PostProcessing> & pprocessing);
	~Window_PostProcessing();

	void draw();
	const std::string windowTitle() const override { return "Post Processing"; }
private:
	std::shared_ptr<PostProcessing> p_postProcessing;;
};
#pragma once
