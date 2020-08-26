#include "Buffer.h"
#include <math.h>
#include <iostream>


/* Float32, passed channels and frames.*/
Buffer::Buffer(unsigned int channels, size_t frameCount)
	:
	m_channelCount{ channels},
	m_format{ paFloat32 },
	m_maxTotalFrames{ frameCount },
	m_channelData{}
{
	clear((int)frameCount);
}


Buffer::Buffer(size_t totalFrames, PaStreamParameters params)
	:
	m_channelCount{ (unsigned)params.channelCount },
	m_format{ params.sampleFormat },
	m_maxTotalFrames{ totalFrames },
	m_channelData{}
{
	clear((int)totalFrames);
}

void Buffer::insertFrames(int totalFrames, const void * inputBuffer, const PaStreamCallbackTimeInfo * timeInfo)
{
	const float *rptr = (const float*)inputBuffer;

	/* TODO: Buffer should not be constructed with buffer size not divisible by channel count.
	-> Check this earlier
	*/
	//if (m_totalFrames % m_channelCount != 0)
	//	std::cout << "WARNING: Framesize is not a multiple of channel numbers!!";

	for (int c = 0; c < m_channelCount; ++c)
	{
		std::deque<float> & channelData = m_channelData[c];
		for (int frameIdx = 0; frameIdx < totalFrames; ++frameIdx)
			channelData.push_back(*(rptr + (frameIdx * m_channelCount) + c));

		size_t extraFrames = (channelData.size() - (size_t)maxChannelFrameCount());
		for (size_t frameIdx = 0; frameIdx < extraFrames; ++frameIdx)
			channelData.pop_front();
	}
}

void Buffer::clear(int totalFrameCount)
{
	m_channelData.clear();

	int maxChannelFrames = (int)m_maxTotalFrames / (int)m_channelCount;
	for (int c = 0; c < m_channelCount; ++c)
	{
		std::deque<float> channelIdxFrames{};
		channelIdxFrames.resize(maxChannelFrames);
		for (int f = 0; f < maxChannelFrames; ++f)
			channelIdxFrames[f] = 0.0f;

		std::pair<int, std::deque<float>> channelIdxKeyVal{ c,channelIdxFrames };
		m_channelData.insert(channelIdxKeyVal);
	}
}

float Buffer::amplitude_peak() const
{
	float peak{ -1.0f };
	for (int c = 0; c < m_channelCount; ++c)
	{
		const std::deque<float> & chData = m_channelData.at(c);
		for (unsigned int f = 0; f < m_channelData.size(); ++f)
			if (peak < chData[f])
				peak = chData[f];
	}
	return peak;
}

float Buffer::amplitude_average() const
{
	float sum{ 0.0f }, average{ 0.0f };
	for (int c = 0; c < m_channelCount; ++c)
	{
		const std::deque<float> & chData = m_channelData.at(c);
		for (unsigned int f = 0; f < m_channelData.size(); ++f)
			sum += chData[f];
	}
	average = sum / (float)m_channelData.size();
	return average;
}

float Buffer::amplitude_minimum() const
{
	float minimum{ 1.0f };
	for (int c = 0; c < m_channelCount; ++c)
	{
		const std::deque<float> & chData = m_channelData.at(c);
		for (unsigned int f = 0; f < m_channelData.size(); ++f)
			if (minimum > chData[f])
				minimum = chData[f];
	}
	return minimum;
}

LockableBuffer::LockableBuffer(Buffer buf)
	: m_buffer{ buf }
{
}

void LockableBuffer::resetBuffer(Buffer buf)
{
	std::lock_guard<std::mutex> lock{ m_bufferMutex };
	m_buffer = buf;
}

Buffer LockableBuffer::buffer()
{
	std::lock_guard<std::mutex> lock{ m_bufferMutex };
	return m_buffer;
}

float hanningWindow(size_t i, size_t s)
{
	return 0.5f*(1.0f - cos(2.0f*3.14159265f*(float)(i) / (float)(s - 1.0f)));
}

std::vector<std::vector<kiss_fft_cpx>> Buffer::fft() const
{
	int nearestPower = 1;
	int channelFrames = ((int)m_maxTotalFrames / (int)m_channelCount);
	while (pow(2, nearestPower) < channelFrames)
		++nearestPower;
	int fftFrames = (int)pow(2, nearestPower);

	std::vector<std::vector<kiss_fft_cpx>> result{};
	for (int c = 0; c < m_channelCount; ++c)
	{
		/*std::vector<std::complex<float>> inputTimeDomain;
		std::vector<std::complex<float>> outputFreqDomain;
*/
		const std::deque<float> chData = m_channelData.at(c);
		std::vector<float> chanResult;
		std::vector<kiss_fft_cpx> inputTimeDomain;
		std::vector<kiss_fft_cpx> outputFreqDomain;
		inputTimeDomain.resize(channelFrames);
		outputFreqDomain.resize(channelFrames);

		for (int sample = 0; sample < chData.size(); sample++)
		{
			inputTimeDomain[sample].r = hanningWindow(sample,chData.size()) * chData[sample];
			inputTimeDomain[sample].i = 0.0f;
		}

		kiss_fft_cfg cfg = kiss_fft_alloc(channelFrames, false, 0, 0);
		kiss_fft(cfg, &inputTimeDomain[0], &outputFreqDomain[0]);

		/* FIX THIS: can you use free()?*/
		delete cfg;

		result.push_back(outputFreqDomain);
	}


	return result;
}

std::vector<float> Buffer::signalPower() const
{
	std::vector<float> result{};
	for (size_t c = 0; c < m_channelCount; ++c)
	{
		const std::deque<float> & ch = m_channelData.at(c);
		double totalSquares{ 0.0 };
		for (std::deque<float>::const_iterator i = ch.begin(); i != ch.end(); ++i)
			totalSquares += (abs(*i * *i));
		float power = (totalSquares / (double)m_channelData.at(c).size());
		result.push_back(power);
	}
	return result;
}

void Buffer::normaliseFFT(std::vector<std::vector<kiss_fft_cpx>>& fftData)
{
	size_t chCount = fftData.size();
	std::vector<kiss_fft_cpx> maxVals{};
	maxVals.resize(chCount, kiss_fft_cpx{ 0.0f,0.0f });
	for (size_t c = 0; c < chCount; ++c)
	{
		//for (size_t s = 0; s < fftData[c].size(); ++s)
		//{
		//	fftData[c][s].r = 10.0f * (float)std::log10(fftData[c][s].r) * 2.0f;
		//	fftData[c][s].i = 10.0f * (float)std::log10(fftData[c][s].i) * 2.0f;
		//}
		for (size_t s = 0; s < fftData[c].size(); ++s)
		{
			if (maxVals[c].i < abs(fftData[c][s].i))
				maxVals[c].i = abs(fftData[c][s].i);
			if (maxVals[c].r < abs(fftData[c][s].r))
				maxVals[c].r = abs(fftData[c][s].r);
		}
		if(maxVals[c].i != 0.0f)
			for (size_t s = 0; s < fftData[c].size(); ++s)
				fftData[c][s].i /= maxVals[c].i;
		if (maxVals[c].r != 0.0f)
			for (size_t s = 0; s < fftData[c].size(); ++s)
				fftData[c][s].r /= maxVals[c].r;
	}
}
