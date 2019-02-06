#pragma once

#include "Recorder.h"
#include "Buffer.h"

#include <memory>

class Settings_AudioInterface;

class Controller_AudioInterface
{
public:

	Controller_AudioInterface(std::shared_ptr<Settings_AudioInterface> & settings);
	~Controller_AudioInterface();

private:
	std::shared_ptr<Settings_AudioInterface> m_settings;
	std::shared_ptr<Recorder> m_realTime;

};

