#pragma once
#include <Rendering/Core/Framebuffer.h>
#include <Rendering/RenderPasses/RenderPass.h>
#include <glm/vec3.hpp>
namespace Dindi
{
	class SSAORenderPass : public RenderPass
	{
	public:
		SSAORenderPass();
		~SSAORenderPass();

		virtual void GenerateOutput(Scene* scene) override final;
		Texture2D* GetRenderTarget() { return m_AOTexture; }
		inline void FeedGBufferData(std::vector<Texture2D*>& gbufferContent) { m_CachedGBuffer = gbufferContent; }
		virtual void TransformAndDraw(Scene* scene) override final;
		Texture2D* GetSSAOTexture() { return m_SSAOTexture; }
	

	private:
		void GenKernelTexture(uint8_t sampleCount);
		void GenNoiseTexture(uint8_t tileSize);

		std::vector<glm::vec3> m_RandomKernel;
		Texture2D* m_AOTexture;
		Texture2D* m_NoiseTexture;
		std::vector<Texture2D*> m_CachedGBuffer;
		Texture2D* m_SSAOTexture;
		Model* m_GBufferQuadModel;
	};
}