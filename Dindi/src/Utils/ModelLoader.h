#pragma once

#include "Visual/Model/Model.h"

namespace Dindi
{
	class ModelLoader
	{
	public:
		static bool Load(std::string filepath, Model& modelToFill);
	private:
		static bool LoadOBJ(std::string filepath, Model& modelToFill);
	};


}