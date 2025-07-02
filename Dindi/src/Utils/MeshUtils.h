#pragma once
#include "Visual/Model/Model.h"

namespace Dindi
{
	class MeshUtil
	{
	public:
		static Model* GenerateQuad(const std::string& vertexpath, const std::string& fragmentPath);
	};
}