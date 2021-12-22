#include "Logger.h"
#include "Core/Core.h"

namespace GLR
{
	std::string Logger::m_levelLabels[4];
	std::ostringstream Logger::m_sstream;
	

	void Logger::Flush()
	{
		//An attempt to avoid using .str without needing
		m_sstream.seekp(0, std::ios::end);
		uint32_t size = (uint32_t)m_sstream.tellp();

		if (size > 0)
		{
			std::cout << m_sstream.str();
			m_sstream.str("");
		}
	}

	void Logger::checkFatal(const Level level)
	{
		if (level == Level::FATAL)
		{
			Flush();
			GLR_ASSERT(false);
		}
	}
}