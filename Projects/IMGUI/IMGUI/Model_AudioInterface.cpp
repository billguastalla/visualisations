#include "Model_AudioInterface.h"
#include "Program.h"

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

Model_AudioInterface::Model_AudioInterface(std::shared_ptr<Settings_AudioInterface> & settings, Program * prog)
	:
	m_settings{ settings },
	m_realTime{ new Recorder{} },
	p_program{ prog },

	m_recordMode{RecordMode::AudioSteam},
	m_recordState{RecordState::Stopped}
{

}

Model_AudioInterface::~Model_AudioInterface()
{

}

void Model_AudioInterface::startStream()
{
	m_realTime->startMonitoring();
	m_recordState = RecordState::Started;
}

void Model_AudioInterface::stopStream()
{
	m_realTime->stopMonitoring();
	m_recordState = RecordState::Stopped;
}

Buffer Model_AudioInterface::buffer()
{
	//Buffer buf{ 512,defaultParams() };
	return m_realTime->getBuffer();
}
