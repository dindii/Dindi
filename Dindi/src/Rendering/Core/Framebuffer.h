#pragma once
#include <Core/Core.h>
#include <Rendering/Core/Common.hpp>
#include <Rendering/Texture/Texture2D.h>

namespace Dindi
{
	//#TODO: this will be extended in the future to support multiple specifications
	//for now, we will just use the default color_depth attachment 


	class Framebuffer
	{
	public:
		Framebuffer(uint16_t width, uint16_t height, RenderTargetDescriptor colorDescriptor = {}, RenderTargetDescriptor depthDescriptor = {});
		Framebuffer();

		virtual ~Framebuffer();

		void Remake();

		void Bind();
		void UnBind();

		void AttachRenderTarget(const Texture2D& rt, FramebufferRenderTargetSlot slot);

		inline uint32_t GetRendererID() const { return m_RendererID; }
		inline uint32_t GetOutputColorImage() const { return m_ColorAttachment; }
		inline uint32_t GetOutputDepthImage() const { return m_DepthAttachment; }
		
		Ref<Texture2D> GetTexture() { return m_Texture; }

	private:
		uint32_t m_RendererID;
		uint32_t m_ColorAttachment, m_DepthAttachment;

		uint16_t m_Width, m_Height;

		RenderTargetDescriptor m_ColorDescriptor;
		RenderTargetDescriptor m_DepthDescriptor;

		Ref<Texture2D> m_Texture;
	};

}