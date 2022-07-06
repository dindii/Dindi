#pragma once
#include "Math/vec3.h"
#include "Visual/Model/Model.h"

namespace Dindi
{
	class DebugRenderer
	{
	public:
		static void Init();
		static void DrawCube(vec3 pos, vec3 color);

	private:
		static Model m_Cube;
	};
}