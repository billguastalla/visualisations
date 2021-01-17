#pragma once
#include "../audio/Recorder.h"

class RecorderUI
{
public:
	RecorderUI(std::shared_ptr<Recorder>& recorder)
		:
		m_recorder{ recorder }
	{
	}

	void draw() const;
private:
	std::shared_ptr<Recorder> m_recorder;
};