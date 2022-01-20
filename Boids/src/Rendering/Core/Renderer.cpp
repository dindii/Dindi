#include "boidspch.h"
#include "Renderer.h"
#include "LowLevelRenderer.h"

namespace Boids
{

	void Renderer::Init()
	{
		GLR_INTERNAL::LowLevelRenderer::Init();
	}

	void Renderer::Draw(Scene* scene)
	{
		//#TODO: Do some work before
		GLR_INTERNAL::LowLevelRenderer::Draw(scene);
	}

	void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		GLR_INTERNAL::LowLevelRenderer::SetViewport(x, y, width, height);
	}

	void Renderer::Clear()
	{
		GLR_INTERNAL::LowLevelRenderer::Clear();
	}

}