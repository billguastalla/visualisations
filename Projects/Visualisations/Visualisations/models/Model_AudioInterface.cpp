#include "Model_AudioInterface.h"
#include "../program/Program.h"

PaStreamParameters defaultParams()
{
	PaStreamParameters params;
	params.channelCount = 2;
	params.device = -1;
	params.hostApiSpecificStreamInfo = nullptr;
	params.sampleFormat = paFloat32;
	params.suggestedLatency = 10.0;
	return params;
}


Model_AudioInterface::Model_AudioInterface(Program * prog)
	:
	p_program{ prog },
	m_realTime{ new Recorder{} },
	m_recordMode{RecordMode::AudioStream}
{
}

Model_AudioInterface::~Model_AudioInterface()
{

}

Buffer Model_AudioInterface::buffer()
{
	return m_realTime->getBuffer();
}
