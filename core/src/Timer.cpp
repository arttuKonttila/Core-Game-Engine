#include "../include/Timer.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

Timer::Timer() :
	m_dRateToSeconds(0.0),
	m_uStartClock(0),
	m_uTickFrequency(0),
	m_fElapsedSeconds(0.0f)
{
	uint64_t rate;
	::QueryPerformanceFrequency((LARGE_INTEGER*)&rate);
	m_dRateToSeconds = 1.0 / (double)rate;
	m_uTickFrequency = rate;
}

void Timer::BeginTimer()
{
	m_uStartClock = GetTicks();
}

void Timer::EndTimer()
{
	const uint64_t endClock = GetTicks();

	if (endClock == m_uStartClock || m_uStartClock == 0) {
		m_fElapsedSeconds = 0.00000001f;
	}
	else {
		const double ticks = (double)(endClock - m_uStartClock);
		m_fElapsedSeconds = (float)(ticks * m_dRateToSeconds);
	}
}

uint64_t Timer::GetTicks()
{
	uint64_t ret = 0;
	::QueryPerformanceCounter((LARGE_INTEGER*)&ret);
	return ret;
}