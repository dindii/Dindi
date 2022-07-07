#pragma once
#include "Math/vec3.h"
#include "Visual/Model/Model.h"

namespace Dindi
{
	namespace Debug
	{
		enum DebugShape
		{
			NONE = -1,
			
			CUBE,
			
			MAX
		};

		class DebugRenderer
		{
		public:
			static void Init();
			static void Draw(DebugShape shape, vec3& pos, vec3& color, float size);
		private:
			static Model* m_DebugShapes[Debug::DebugShape::MAX];
		};
	}

}