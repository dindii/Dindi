#pragma once
#include "Math/vec3.h"
#include "Visual/Model/Model.h"

//#IMPORTANT: - Use it only for debugging purposes!
//This debug renderer is not meant to be fast but flexible and immediate. It will issue a draw call every time you call "Draw()"
//as well as upload some uniforms. Keep in mind that this must be only used in debug mode and it will be stripped in other build types.

namespace Dindi
{
	namespace Debug
	{
		enum EDebugShape
		{
			NONE = -1,
			
			CUBE,
			LINE,

			MAX
		};

		enum EDebugRenderFlags : uint32_t
		{
			DEFAULT          = BIT(0),
			WIREFRAME        = BIT(1),
			NO_DEPTH_TESTING = BIT(2)
		};

		struct DebugShapeContext
		{
		public:
			EDebugShape shapeType = EDebugShape::NONE;
			vec3 firstPosition;
			vec3 secondPosition;
			vec3 shapeColor;
			float shapeSize = 1.0f;
			uint32_t shapeLifetime = 0;
			uint32_t shapeRenderFlags = EDebugRenderFlags::DEFAULT;

			bool Tick();

		private:
			bool m_FirstTick = true;
			time_t m_AliveUntil = 0;
		};

		class DebugRenderer
		{
		public:
#ifdef DINDI_DEBUG
			static void Init();
			static void SubmitDraw();
			static void DrawShape(const DebugShapeContext& debugShapeContext);
			static void ClearQueue();


		private:
			/*
			* @briefing: Draws a shape in the desired position, with the desired color.
			* @param - The desired shape to draw (CUBEs, LINEs etc)
			* @param - Position of the shape
			* @param [OPTIONAL] - This is used for LINES only, it is the end of the line. Ignore if you are not using LINE as the shape.
			* @param - Color of the shape
			* @param - Size of the shape. If the shape is a LINE, it will be the line thickness
			* @param - Flags to render. You can have WIREFRAME mode and NO DEPTH TESTING that will always pass the depth test (it will be in front of everything).
			*/
			static void ImmediateDebugDrawShape(const DebugShapeContext& debugShapeContext);
#else				
			static void Init() {};
			static void SubmitDraw() {};
			static void DrawShape(const DebugShapeContext& debugShapeContext) {};
			static void ImmediateDebugDrawShape(const DebugShapeContext& debugShapeContext) {};
			static void ClearQueue() {};
#endif

#ifdef DINDI_DEBUG
		private:
			static Model* m_DebugShapes[Debug::EDebugShape::MAX];
			static std::vector<DebugShapeContext> m_OnFlyDrawCalls;
#endif
		};
	}

}