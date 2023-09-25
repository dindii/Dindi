#pragma once
#include <Core/Core.h>
#include <Rendering/Core/Common.h>
#include <Rendering/Texture/Texture2D.h>

namespace Dindi
{
	//#TODO: this will be extended in the future to support multiple specifications
	//for now, we will just use the default color_depth attachment 


	class Framebuffer
	{
	public:
		Framebuffer(uint16_t width, uint16_t height, RenderTargetDescriptor colorDescriptor = {}, RenderTargetDescriptor depthDescriptor = {});
		virtual ~Framebuffer();

		void Remake();

		void Bind();
		void UnBind();

		inline uint32_t GetRendererID() const { return m_RendererID; }
		inline uint32_t GetOutputColorImage() const { return m_ColorAttachment; }
		inline uint32_t GetOutputDepthImage() const { return m_DepthAttachment; }
		

	private:
		uint32_t m_RendererID;
		uint32_t m_ColorAttachment, m_DepthAttachment;

		uint16_t m_Width, m_Height;

		RenderTargetDescriptor m_ColorDescriptor;
		RenderTargetDescriptor m_DepthDescriptor;
	};

}