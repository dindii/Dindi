#pragma once

#include <Rendering/Core/Framebuffer.h>
#include <glm/mat4x4.hpp>
#include <Rendering/Scene/Scene.h>

namespace Dindi
{
	class RenderPass
	{
	public:
		virtual void GenerateOutput(Scene* scene) = 0;	
		virtual uint32_t GetOutputColorHandle() { return m_Framebuffer->GetOutputColorImage(); }
	protected:
		virtual void TransformAndDraw(Scene* scene) = 0;
		
		Framebuffer* m_Framebuffer = nullptr;
		Ref<Shader> m_Shader;
	};
}