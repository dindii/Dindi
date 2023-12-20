#pragma once
#include "Rendering/Scene/Scene.h"



namespace Dindi
{

struct GraphicsDefinitions;

	class Renderer
	{
	public:
		Renderer() = default;

		static void Init();
		static void Draw(Scene* scene);
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void OnContextResize(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static uint32_t GetScreenOutputHandle();
		static std::vector<Texture2D*>& GetShadowMap();
		

		static GraphicsDefinitions& GetGraphicsDefinitions();

		static void Clear(bool colorAttach = true, bool depthAttach = true);
	};

}