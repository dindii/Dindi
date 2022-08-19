#pragma once
#include "Math/vec3.h"
#include "Visual/Model/Model.h"

//#IMPORTANT: - Use it only for debugging purposes!
//This debug renderer is not meant to be fast but flexible and immediate. It will issue a draw call everytime you call "Draw()"
//as well as upload some uniforms. Keep in mind that this must be only used in debug mode and it will be stripped in other build types.

namespace Dindi
{
	namespace Debug
	{
		enum EDebugShape
		{
			NONE = -1,
			
			CUBE,
			
			MAX
		};

		enum EDebugRenderFlags : uint32_t
		{
			DEFAULT          = BIT(0),
			WIREFRAME        = BIT(1),
			NO_DEPTH_TESTING = BIT(2)
		};

		class DebugRenderer
		{
		public:
			static void Init();
#ifdef DINDI_DEBUG
			static void Draw(EDebugShape shape, vec3& pos, vec3& color, float size, uint32_t flags = EDebugRenderFlags::DEFAULT);
#else
			static void Draw(EDebugShape shape, vec3& pos, vec3& color, float size, uint32_t flags = EDebugRenderFlags::DEFAULT) {};
#endif
		private:
			static Model* m_DebugShapes[Debug::EDebugShape::MAX];
		};
	}

}