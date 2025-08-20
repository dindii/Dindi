#pragma once
#include <Rendering/Core/Framebuffer.h>
#include <Rendering/RenderPasses/RenderPass.h>
#include <glm/vec3.hpp>

namespace Dindi
{
	class SimpleGaussianBlurRenderPass : public RenderPass
	{
	public:
		SimpleGaussianBlurRenderPass();
		~SimpleGaussianBlurRenderPass();

		virtual void GenerateOutput(Scene* scene) override final;
		virtual void TransformAndDraw(Scene* scene) override final;

		inline void FeedTextureToBlur(Texture2D* textureToBlur) { m_CachedTextureToBlur = textureToBlur; }
		Texture2D* GetBlurTexture() { return m_CachedTextureToBlur; }

	private:
		Texture2D* m_CachedTextureToBlur;
		Texture2D* m_BlurTexture;
		Model* m_GBufferQuadModel;
	};
}