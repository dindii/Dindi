#pragma once

#include <string_view>
#include <chrono>

#define SCOPED_PROFILE(title, printOnEnd) TimerProf timer##__LINE__(title, printOnEnd)

namespace Dindi
{
	class TimerProf
	{
	public:
		TimerProf(const std::string_view& scopeTitle, bool printOnEnd = false);
		~TimerProf();

	private:
		bool m_PrintOnEnd;
		std::string_view m_Title;

		std::chrono::time_point<std::chrono::steady_clock> m_StartTimer;
	};
}