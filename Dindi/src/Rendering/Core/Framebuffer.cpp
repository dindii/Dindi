#include "Dindipch.h"
#include <Rendering/Core/Common.hpp>
#include "Framebuffer.h"
#include <Utils/Logger.h>
#include "Platform/Window.h"
#include <Core/Application.h>

namespace Dindi
{
	Framebuffer::Framebuffer(uint16_t width, uint16_t height, RenderTargetDescriptor colorDescriptor, RenderTargetDescriptor depthDescriptor) : m_RendererID(0), m_ColorAttachment(0), m_DepthAttachment(0),
		m_Width(width), m_Height(height), m_ColorDescriptor(colorDescriptor), m_DepthDescriptor(depthDescriptor)
	{
		Remake();
	}

	Framebuffer::Framebuffer() : m_RendererID(0), m_ColorAttachment(0), m_DepthAttachment(0), m_Width(0), m_Height(0)
	{
		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);


		UnBind();
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColorAttachment);
	}

	void Framebuffer::Remake()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteTextures(1, &m_DepthAttachment);


			m_RendererID = 0;
			m_ColorAttachment = 0;
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// -------------------- Color attachment
		if (m_ColorDescriptor.internalFormat != RenderTargetInternalFormat::DND_NONE)
		{
			glGenTextures(1, &m_ColorAttachment);
			glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);

			Application& app = Application::GetInstance();

			RenderTargetFormat format = GetFormatFromInternalDataType(m_ColorDescriptor.internalFormat);

			glTexImage2D(GL_TEXTURE_2D, 0, m_ColorDescriptor.internalFormat, m_Width, m_Height, 0, format, m_ColorDescriptor.type, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);
		}
		// --------------------




		// -------------------- Depth attachment
		if (m_DepthDescriptor.internalFormat != RenderTargetInternalFormat::DND_NONE)
		{
			glGenTextures(1 , &m_DepthAttachment);
			glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

			RenderTargetFormat depthFormat = GetFormatFromInternalDataType(m_DepthDescriptor.internalFormat);

			glTexImage2D(GL_TEXTURE_2D, 0, m_DepthDescriptor.internalFormat, m_Width, m_Height, 0, depthFormat, m_DepthDescriptor.type, NULL);
		
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
			
			//If we only have this depth attachment, we have to explicitly tell OGL that we doesn't want a color buffer
			if (m_ColorDescriptor.internalFormat == RenderTargetInternalFormat::DND_NONE)
			{
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
			}
			// --------------------
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			DND_LOG_ERROR("Framebuffer is not complete! Make sure to attach something to it for later use.");

		UnBind();
	}

	void Framebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}
	void Framebuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::AttachRenderTarget(const Texture2D& rt, FramebufferRenderTargetSlot slot)
	{
		int32_t attachmentType = slot == FramebufferRenderTargetSlot::COLOR ? GL_COLOR_ATTACHMENT0 : GL_DEPTH_ATTACHMENT;
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, rt.GetID(), 0);
	}

}