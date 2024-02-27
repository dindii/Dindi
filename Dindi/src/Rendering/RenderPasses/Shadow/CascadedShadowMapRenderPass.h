#pragma once
#include <Rendering/Core/Framebuffer.h>
#include <Core/Core.h>
#include <glm/mat4x4.hpp>
#include <Rendering/Scene/Scene.h>

namespace Dindi
{
	class CSMRenderPass
	{
	public:
		CSMRenderPass();
		~CSMRenderPass();

		void GenerateOutput(Scene* scene);
		std::vector<Texture2D*>& GetRenderTarget() { return m_CSMTextures; }
		std::vector<glm::mat4>& GetTransforms() { return m_CSMLightOrthographicViewTransform; }
	
		static uint32_t GetRandomAnglesTexture() { return RandomAnglesTexture; }
		void GenerateRandom3DTexture();
	private:
		void RecalculateProjectionMatrix();
		void TransformAndDraw(Scene* scene);
		void UpdateFrustumCorners();

	private:
		std::vector<glm::mat4> m_CSMPerspectiveProjections;
		std::vector<glm::mat4> m_CSMLightOrthographicViewTransform;
		Framebuffer* m_CSMFramebuffer = nullptr;
		Ref<Shader> m_CSMShader;

		static inline uint32_t RandomAnglesTexture = 0;

		std::vector<Texture2D*> m_CSMTextures;
	};
}