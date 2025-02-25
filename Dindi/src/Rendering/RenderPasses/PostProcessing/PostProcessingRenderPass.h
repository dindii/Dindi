#pragma once

#include <Rendering/RenderPasses/RenderPass.h>

namespace Dindi
{
	class PostProcessingRenderPass : public RenderPass
	{
	public:
		PostProcessingRenderPass();
		~PostProcessingRenderPass() = default;

		Texture2D* GetRenderTarget() { return m_RenderTarget; }
		void FeedRawRenderData(Texture2D* rawRender) { m_CachedRawRender = rawRender; }
		virtual void GenerateOutput(Scene* scene) override final;
	protected:
		virtual void TransformAndDraw(Scene* scene) override final;

	private:
		Model* m_ScreenQuadModel;
		Texture2D* m_CachedRawRender;
		Texture2D* m_RenderTarget;

	};
}
