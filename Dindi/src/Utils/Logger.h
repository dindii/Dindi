#pragma once
#include <string>
#include <sstream>
#include <iostream>

//This logger is not recommended for tracer purposes (std::cout), it grants great flexibility and it is called once per frame.
//Only works in DEBUG mode.

namespace Dindi
{
	class Logger
	{
	public:
		enum Level : uint8_t
		{
			DND_TRACE = 0,
			DND_WARNING,
			DND_ERROR,
			DND_FATAL
		};


		Logger() = delete;


		static void Init()
		{
			m_levelLabels[Level::DND_TRACE] = "TRACE";
			m_levelLabels[Level::DND_WARNING] = "WARNING";
			m_levelLabels[Level::DND_ERROR] = "ERROR";
			m_levelLabels[Level::DND_FATAL] = "FATAL";
		}

		~Logger() = default;

		template<typename ... Args>
		static void Log(Level level, Args&& ... args)
		{
			m_sstream << m_levelLabels[level] << ": ";
			(m_sstream << ... << std::forward<Args>(args)) << '\n';

			checkFatal(level);
		}

		//Print and clear the content of m_sstream;
		static void Flush();

	private:
		//Check if we should throw an error
		static void checkFatal(const Level level);

		static std::string m_levelLabels[4];
		static std::ostringstream m_sstream;
	};
}


#ifndef DINDI_DEBUG
#define DND_LOG_TRACE(...) 
#define DND_LOG_WARNING(...)
#define DND_LOG_ERROR(...) 
#define DND_LOG_FATAL(...) 
#else
#define DND_LOG_TRACE(...)   Dindi::Logger::Log(Dindi::Logger::Level::DND_TRACE, __VA_ARGS__);
#define DND_LOG_WARNING(...) Dindi::Logger::Log(Dindi::Logger::Level::DND_WARNING, __LINE__, " ", __FILE__, __VA_ARGS__)
#define DND_LOG_ERROR(...)   Dindi::Logger::Log(Dindi::Logger::Level::DND_ERROR,  __LINE__, " ", __FILE__, __VA_ARGS__)
#define DND_LOG_FATAL(...)   Dindi::Logger::Log(Dindi::Logger::Level::DND_FATAL,  __LINE__, " ", __FILE__, __VA_ARGS__)
#endif

