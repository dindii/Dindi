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
		uint32_t GetOutput() { return m_CSMTextures[0]; };
		
		
		glm::mat4 m_CascadesOrthographicProjections[DND_CASCADED_SHADOW_MAP_LEVELS];
	private:
		void RecalculateProjectionMatrix();
		void TransformAndDraw(Scene* scene);

		void UpdateFrustumCorners();

	private:
		Framebuffer* m_CSMFramebuffer = nullptr; 
		RenderTargetDescriptor* m_CacheDepthDescriptor = nullptr;
		Ref<Shader> m_CSMShader;
		
		
		uint32_t m_CSMTextures[DND_CASCADED_SHADOW_MAP_LEVELS];

		//#TOREVERT
		//glm::mat4 m_CascadesOrthographicProjections[DND_CASCADED_SHADOW_MAP_LEVELS];
		glm::mat4 m_CSMLightView; 
		


	};
}


