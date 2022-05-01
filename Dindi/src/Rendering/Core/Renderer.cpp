#include "Dindipch.h"
#include "Renderer.h"
#include "LowLevelRenderer.h"

namespace Dindi
{

	void Renderer::Init()
	{
		DND_INTERNAL::LowLevelRenderer::Init();
	}

	void Renderer::Draw(Scene* scene)
	{
		//#TODO: Do some work before
		DND_INTERNAL::LowLevelRenderer::Draw(scene);
	}

	void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		DND_INTERNAL::LowLevelRenderer::SetViewport(x, y, width, height);
	}

	void Renderer::Clear()
	{
		DND_INTERNAL::LowLevelRenderer::Clear();
	}

}