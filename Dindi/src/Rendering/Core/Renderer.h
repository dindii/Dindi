#pragma once
#include "Rendering/Scene/Scene.h"
namespace Dindi
{
	class Renderer
	{
	public:
		Renderer() = default;

		static void Init();
		static void Draw(Scene* scene);
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void Clear();
	};

}