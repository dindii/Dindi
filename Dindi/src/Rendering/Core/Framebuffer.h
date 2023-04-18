#pragma once
#include <Core/Core.h>
#include <Rendering/Texture/Texture2D.h>

namespace Dindi
{
	//#TODO: this will be extended in the future to support multiple specifications
	//for now, we will just use the default color_depth attachment 
	class Framebuffer
	{
	public:
		Framebuffer();
		virtual ~Framebuffer();

		void Remake();

		void Bind();
		void UnBind();

		inline uint32_t GetRendererID() const { return m_RendererID; }
		inline uint32_t GetOutputImage() const { return m_ColorAttachment; }

	private:
		uint32_t m_RendererID;
		uint32_t m_ColorAttachment, m_DepthAttachment;
	};

}