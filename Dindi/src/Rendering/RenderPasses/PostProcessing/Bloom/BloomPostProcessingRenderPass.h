#pragma once
#include <Rendering/RenderPasses/RenderPass.h>
#include <Utils/AssetManager.h>
#include <stdint.h>

namespace Dindi
{
	class BloomPostProcessingRenderPass : public RenderPass
	{
	public:
		BloomPostProcessingRenderPass(uint8_t mipCount);
		BloomPostProcessingRenderPass() = default;

		inline std::vector<Texture2D*>& GetRenderTargetMips() { return m_RenderTargetMips; }
		inline void FeedSourceHDRBuffer(Texture2D* rawRender) { m_SourceHDRBuffer = rawRender; }
		virtual void GenerateOutput(Scene* scene) override final;
	private:
		virtual void TransformAndDraw(Scene* scene) override final;

		void DownsampleMips();
		void UpsampleMips(float radii);

	private:
		uint8_t m_MipCount = 1;
		Model* m_ScreenQuadModel;
		Texture2D* m_SourceHDRBuffer;
		std::vector<Texture2D*> m_RenderTargetMips;

		Ref<Shader> m_DownsampleShader;
		Ref<Shader> m_UpsampleShader;
	};
}
