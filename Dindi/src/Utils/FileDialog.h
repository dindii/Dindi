#pragma once
#include "Core/Core.h"
#include <string >


namespace Dindi
{
	class FileDialog
	{
	public:
		static std::string OpenFile(const char* filter);
	};
}