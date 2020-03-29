#pragma once


class Settings_AudioInterface
{
public:
	Settings_AudioInterface();
	~Settings_AudioInterface();

	int sampleRate() const { return m_sampleRate; }
private:
	int m_sampleRate;
};
