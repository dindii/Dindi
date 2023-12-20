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

	void Renderer::OnContextResize(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		if (width <= 0 || height <= 0)
			return;

		DND_INTERNAL::LowLevelRenderer::SetViewport(x, y, width, height);
		DND_INTERNAL::LowLevelRenderer::RemakeFramebuffers(width, height);
	}

	uint32_t Renderer::GetScreenOutputHandle()
	{
		return DND_INTERNAL::LowLevelRenderer::GetScreenOutputHandle();
	}

	std::vector<Texture2D*>& Renderer::GetShadowMap()
	{
		return DND_INTERNAL::LowLevelRenderer::GetShadowMap();
	}

	Dindi::GraphicsDefinitions& Renderer::GetGraphicsDefinitions()
	{
		return DND_INTERNAL::LowLevelRenderer::GetGraphicsDefinitions();
	}

	void Renderer::Clear(bool colorAttach, bool depthAttach)
	{
		DND_INTERNAL::LowLevelRenderer::Clear(colorAttach, depthAttach);
	}

}