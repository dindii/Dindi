#pragma once
#include <Rendering/Core/Framebuffer.h>
#include <Core/Core.h>
#include <glm/mat4x4.hpp>
#include <Rendering/Scene/Scene.h>
#include <Rendering/RenderPasses/RenderPass.h>
namespace Dindi
{
	class CSMRenderPass : public RenderPass
	{
	public:
		CSMRenderPass();
		~CSMRenderPass();

		virtual void GenerateOutput(Scene* scene) override final;
		std::vector<Texture2D*>& GetRenderTarget() { return m_CSMTextures; }
		std::vector<glm::mat4>& GetTransforms() { return m_CSMLightOrthographicViewTransform; }
	
		static uint32_t GetRandomAnglesTexture() { return RandomAnglesTexture; }
		void GenerateRandom3DTexture(int windowSize, int filterSize);
	private:
		void RecalculateProjectionMatrix();
		virtual void TransformAndDraw(Scene* scene) override final;
		void UpdateFrustumCorners();

	private:
		std::vector<glm::mat4> m_CSMPerspectiveProjections;
		std::vector<glm::mat4> m_CSMLightOrthographicViewTransform;

		static inline uint32_t RandomAnglesTexture = 0;

		std::vector<Texture2D*> m_CSMTextures;
	};
}