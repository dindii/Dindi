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

		glm::mat4 m_CSMLightOrthographicViewTransform[DND_CASCADED_SHADOW_MAP_LEVELS];
	private:
		void RecalculateProjectionMatrix();
		void TransformAndDraw(Scene* scene);

		void UpdateFrustumCorners();

		std::vector<glm::mat4> m_CSMPerspectiveProjections;

	private:
		Framebuffer* m_CSMFramebuffer = nullptr;
		Ref<Shader> m_CSMShader;


		std::vector<Texture2D*> m_CSMTextures;

		glm::mat4 m_CSMLightView;

	};
}