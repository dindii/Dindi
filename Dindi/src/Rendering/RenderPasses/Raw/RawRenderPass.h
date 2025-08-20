#pragma once

#include <Rendering/RenderPasses/RenderPass.h>

namespace Dindi
{
	class RawRenderPass : public RenderPass
	{
	public:
		RawRenderPass();
		~RawRenderPass();

		inline void FeedCSMData(std::vector<glm::mat4>& shadowTransforms) { m_CachedShadowTransforms = shadowTransforms; }
		inline void FeedGBufferData(std::vector<Texture2D*>& gbufferContent) { m_CachedGBuffer = gbufferContent; }
		inline void FeedSSAOData(Texture2D* ssaoTex) { m_CachedSSAOTexture = ssaoTex; }

		Texture2D* GetRenderTarget() { return m_RenderTarget; }
		virtual void GenerateOutput(Scene* scene) override final;
	protected:
		virtual void TransformAndDraw(Scene* scene) override final;

	private:
		void TransformAndDrawDebugShapes(Scene* scene);
		std::vector<glm::mat4> m_CachedShadowTransforms;
		Texture2D* m_RenderTarget;
		Model* m_GBufferQuadModel;
		Texture2D* m_CachedSSAOTexture;
		std::vector<Texture2D*> m_CachedGBuffer;
	};
}