#pragma once
#include "Window_Abstract.h"
#include <memory>

class Model_Transport;

class Window_Transport : public Window_Abstract
{
public:
	Window_Transport(std::shared_ptr<Model_Transport> transport)
		: m_transport{ transport } {}

	void draw() override;
	const std::string windowTitle() const override { return std::string{ "Transport" }; }
private:
	std::shared_ptr<Model_Transport> m_transport;
};
