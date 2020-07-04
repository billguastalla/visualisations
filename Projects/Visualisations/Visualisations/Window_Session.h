#pragma once
#include "Window_Abstract.h"
#include <memory>
class Model_Session;

class Window_Session : public Window_Abstract
{
public:
	Window_Session(std::shared_ptr<Model_Session> & session);

	void draw() override;
	const std::string windowTitle() const override { return "Session"; };
private:
	std::shared_ptr<Model_Session> m_session;

	char ui_fileName[256];
	char ui_filePath[256];
	char ui_title[256];
};