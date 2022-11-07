#include "Dindipch.h"
#include "Framebuffer.h"
#include <glad/glad.h>
#include <Utils/Logger.h>

namespace Dindi
{
	Framebuffer::Framebuffer() : m_RendererID(0), m_ColorAttachment(0), m_DepthAttachment(0)
	{
		Remake();
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
		glGenTextures(1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1366, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);
		// --------------------

		// -------------------- Depth attachment
		glGenTextures(1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 1366, 768, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
		// --------------------

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			DND_LOG_ERROR("Failed to create OpenGL Framebuffer!");

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
}