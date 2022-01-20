#pragma once

#include "Math/vec4.h"
#include "Rendering/Scene/Scene.h"

namespace Boids
{

	namespace GLR_INTERNAL
	{
		class LowLevelRenderer
		{
		public:
			LowLevelRenderer();
			~LowLevelRenderer();

			static void Init();
			static void Draw(Scene* scene);
			static void Clear(const bool ColorBuffer = true, const bool DepthBuffer = true);
			static void SetClearColor(const vec4& color);
			static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
			static unsigned int GenMesh(unsigned int nMeshes = 1);
			static void SetWireframeMode(bool cond);
			static void DelMesh(unsigned int Mesh, unsigned int nMeshes = 1);
		};
	}

}