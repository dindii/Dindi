#pragma once
#include <string>
#include <sstream>
#include <iostream>

//This logger is not recommended for tracer purposes (std::cout), it grants great flexibility and it is called once per frame.
//Only works in DEBUG mode.

namespace Boids
{
	class Logger
	{
	public:
		enum Level : uint8_t
		{
			TRACE = 0,
			WARNING,
			ERROR,
			FATAL
		};


		Logger()
		{
			m_levelLabels[Level::TRACE] = "TRACE";
			m_levelLabels[Level::WARNING] = "WARNING";
			m_levelLabels[Level::ERROR] = "ERROR";
			m_levelLabels[Level::FATAL] = "FATAL";
		}

		~Logger() = default;

		template<typename ... Args>
		inline static void Log(Level level, Args&& ... args)
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


#ifdef NDEBUG
#define GLR_LOG_TRACE(...) 
#define GLR_LOG_WARNING(...)
#define GLR_LOG_ERROR(...) 
#define GLR_LOG_FATAL(...) 
#else
#define GLR_LOG_TRACE(...)   Boids::Logger::Log(Boids::Logger::Level::TRACE, __VA_ARGS__);
#define GLR_LOG_WARNING(...) Boids::Logger::Log(Boids::Logger::Level::WARNING, __LINE__, " ", __FILE__, __VA_ARGS__)
#define GLR_LOG_ERROR(...)   Boids::Logger::Log(Boids::Logger::Level::ERROR,  __LINE__, " ", __FILE__, __VA_ARGS__)
#define GLR_LOG_FATAL(...)   Boids::Logger::Log(Boids::Logger::Level::FATAL,  __LINE__, " ", __FILE__, __VA_ARGS__)
#endif

