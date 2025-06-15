#pragma once
#include <Dindipch.h>
#include "TimerProf.h"
#include <chrono>
#include <Utils/Logger.h>

namespace Dindi
{
	TimerProf::TimerProf(const std::string_view& scopeTitle, bool printOnEnd /*= false*/) : m_Title(scopeTitle),  m_PrintOnEnd(printOnEnd)
	{
		m_StartTimer = std::chrono::high_resolution_clock::now();
	}

	TimerProf::~TimerProf()
	{
		std::chrono::time_point<std::chrono::steady_clock> endTime = std::chrono::high_resolution_clock::now();

		time_t start = std::chrono::time_point_cast<std::chrono::milliseconds>(m_StartTimer).time_since_epoch().count();
		time_t end = std::chrono::time_point_cast<std::chrono::milliseconds>(endTime).time_since_epoch().count();

		uint64_t timePassed = end - start;
		
		if (m_PrintOnEnd)
			DND_LOG_TRACE("Profile Report: ", m_Title.data(), " took ", timePassed, " ms to finish.");
	}

}
