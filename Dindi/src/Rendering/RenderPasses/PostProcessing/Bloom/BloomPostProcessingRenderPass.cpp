#include <Dindipch.h>
#include "BloomPostProcessingRenderPass.h"
#include <Platform/Window.h>
#include <core/Application.h>
#include <utils/DNDAssert.h>
#include <Rendering/Core/LowLevelRenderer.h>

#include <GUI/UILayer.h>

namespace Dindi
{
	static float quadVertices[] = {
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f
	};

	static float quadUVs[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	Ref<Shader> copyShader = nullptr;

	BloomPostProcessingRenderPass::BloomPostProcessingRenderPass(uint8_t mipCount)
	{
		DND_ASSERT(mipCount > 0, "Bloom Post Processing negative MipCount!");

		m_MipCount = mipCount;

		m_UpscaleTargetMips.reserve(m_MipCount);

		m_ScreenQuadModel = new Model();

		Mesh*  cubeDebugMesh = new Mesh(RESOURCES_PATH "Resources/Shaders/PostProcessing/PostProcessingVertex.shader", RESOURCES_PATH "Resources/Shaders/PostProcessing/PostProcessingFrag.shader");

		std::vector<glm::vec3> quadVerticesMem;
		std::vector<glm::vec2> quadUVsMem;

		quadVerticesMem.resize(4);
		quadUVsMem.resize(4);

		memcpy(&quadVerticesMem[0], quadVertices, 4 * sizeof(glm::vec3));
		memcpy(&quadUVsMem[0], quadUVs, 4 * sizeof(glm::vec2));

		cubeDebugMesh->SetVertexPositionData(std::move(quadVerticesMem));
		cubeDebugMesh->SetTextureCoordData(std::move(quadUVsMem));

		m_ScreenQuadModel->AddMesh(cubeDebugMesh);

		Application& app = Application::GetInstance();

		uint32_t nwidth = app.GetWindow()->GetWidth();
		uint32_t nheight = app.GetWindow()->GetHeight();

		RenderTargetDescriptor bloomPostProcessingColorDesc;
		bloomPostProcessingColorDesc.internalFormat = RenderTargetInternalFormat::DND_COLOR_RGBA32F;
		bloomPostProcessingColorDesc.type = RenderTargetDataType::DND_FLOAT;

		m_Framebuffer = new Framebuffer(nwidth, nheight, bloomPostProcessingColorDesc);

		glm::vec2 mipSize(nwidth, nheight);

		for (uint8_t i = 0; i < m_MipCount; i++)
		{
			mipSize *= 0.5f;

			bloomPostProcessingColorDesc.width =  (uint32_t)mipSize.x;
			bloomPostProcessingColorDesc.height = (uint32_t)mipSize.y;
			bloomPostProcessingColorDesc.magFilter = RenderTargetMagMinFilter::LINEAR;
			bloomPostProcessingColorDesc.minFilter = RenderTargetMagMinFilter::LINEAR;
			bloomPostProcessingColorDesc.wrapU = RenderTargetWrapMode::STRETCH;
			bloomPostProcessingColorDesc.wrapV = RenderTargetWrapMode::STRETCH;

			Texture2D* temp = new Texture2D(bloomPostProcessingColorDesc);
			m_UpscaleTargetMips.push_back(temp);

			Texture2D* tempDown = new Texture2D(bloomPostProcessingColorDesc);
			m_DownscaleTargetMips.push_back(tempDown);
		}

		m_Framebuffer->AttachRenderTarget(*m_UpscaleTargetMips[0], FramebufferRenderTargetSlot::COLOR);
	
		m_DownsampleShader = Shader::Load(RESOURCES_PATH "Resources/Shaders/PostProcessing/Bloom/DownscalingBloomVert.shader", RESOURCES_PATH "Resources/Shaders/PostProcessing/Bloom/DownscalingBloomFrag.shader");
		m_UpsampleShader = Shader::Load(RESOURCES_PATH "Resources/Shaders/PostProcessing/Bloom/UpscalingBloomVert.shader", RESOURCES_PATH "Resources/Shaders/PostProcessing/Bloom/UpscalingBloomFrag.shader");
		copyShader = Shader::Load(RESOURCES_PATH "Resources/Shaders/CopyVert.shader", RESOURCES_PATH "Resources/Shaders/CopyFrag.shader");
	}

	void BloomPostProcessingRenderPass::GenerateOutput(Scene* scene)
	{
		
		m_Framebuffer->Bind();
	
		DownsampleMips();
		CopyDownscaleTextures();
		UpsampleMips(0.005f);

		Application& app = Application::GetInstance();
	
		uint32_t nwidth = app.GetWindow()->GetWidth();
		uint32_t nheight = app.GetWindow()->GetHeight();
	
		glViewport(0, 0, nwidth, nheight);
	
		m_Framebuffer->UnBind();
		m_UpsampleShader->UnBind();
	}

	void BloomPostProcessingRenderPass::TransformAndDraw(Scene* scene)
	{
		
	}

	void BloomPostProcessingRenderPass::CopyDownscaleTextures()
	{
		//no-op
	}

	void BloomPostProcessingRenderPass::DownsampleMips()
	{
		 
		Application& app = Application::GetInstance();

		uint32_t nwidth = app.GetWindow()->GetWidth();
		uint32_t nheight = app.GetWindow()->GetHeight();

		m_DownsampleShader->Bind();
		m_DownsampleShader->UploadInt("u_SrcTexture", ERenderingMapSlot::BloomSource);
		m_DownsampleShader->UploadUniformFloat2("u_SrcResolution", glm::vec2(float(nwidth), float(nheight)));
		m_SourceHDRBuffer->Bind(ERenderingMapSlot::BloomSource);

		for (uint8_t i = 0; i < m_MipCount; i++)
		{
			const Texture2D* mip = m_UpscaleTargetMips[i];

			glViewport(0, 0, mip->GetWidth(), mip->GetHeight());
			m_Framebuffer->AttachRenderTarget(*mip, FramebufferRenderTargetSlot::COLOR);

			i == 0 ? m_DownsampleShader->UploadInt("u_PreFilter", 1) : m_DownsampleShader->UploadInt("u_PreFilter", 0);
			m_DownsampleShader->UploadUniformFloat("u_Threshold", 1.0f);

			Mesh* mesh = m_ScreenQuadModel->GetMeshes()[0];
			glBindVertexArray(mesh->GetVertexArrayObjectID());
			glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVertexBufferObjectID());
			glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh->GetVertexCount());

			glFlush();

			/*test*/
			copyShader->Bind();
			m_Framebuffer->AttachRenderTarget(*m_DownscaleTargetMips[i], FramebufferRenderTargetSlot::COLOR);
			copyShader->UploadInt("u_SrcCopy", ERenderingMapSlot::BloomSource);
			mip->Bind(ERenderingMapSlot::BloomSource);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh->GetVertexCount());
			copyShader->UnBind();

			/*test*/
			
			m_DownsampleShader->Bind();
			m_DownsampleShader->UploadUniformFloat2("u_SrcResolution", glm::vec2(float(mip->GetWidth()), float(mip->GetHeight())));
			m_DownsampleShader->UploadInt("u_SrcTexture", ERenderingMapSlot::BloomSource);
			mip->Bind(ERenderingMapSlot::BloomSource);
		}

		static bool once = false;
		if (!once)
		{
			for (uint8_t i = 0; i < m_MipCount; i++)
			{
				once = true;
				uint32_t id = m_DownscaleTargetMips[i]->GetID();
				Application::GetInstance().GetUILayer()->DebugImageUI(id);
				
			}
		}
	}

	void BloomPostProcessingRenderPass::UpsampleMips(float radii)
	{
		 
		Application& app = Application::GetInstance();

		uint32_t nwidth = app.GetWindow()->GetWidth();
		uint32_t nheight = app.GetWindow()->GetHeight();

		m_UpsampleShader->Bind();
		m_UpsampleShader->UploadInt("u_SrcTexture", ERenderingMapSlot::BloomSource);
		m_UpsampleShader->UploadUniformFloat("u_FilterRadius", radii);
		
		m_SourceHDRBuffer->Bind(ERenderingMapSlot::BloomSource);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);


		for (uint8_t i = m_MipCount - 1; i > 0; i--)
		{
			const Texture2D* mip = m_UpscaleTargetMips[i];
			const Texture2D* nextMip = m_UpscaleTargetMips[i - 1];


			m_UpsampleShader->UploadInt("u_SrcTexture", ERenderingMapSlot::BloomSource);
			mip->Bind(ERenderingMapSlot::BloomSource);

			m_UpsampleShader->UploadInt("u_DownsampleSrcTexture", ERenderingMapSlot::BloomHotGlowSrc);
			m_DownscaleTargetMips[i]->Bind(ERenderingMapSlot::BloomHotGlowSrc);

			glViewport(0, 0, nextMip->GetWidth(), nextMip->GetHeight());
			m_Framebuffer->AttachRenderTarget(*nextMip, FramebufferRenderTargetSlot::COLOR);

			DND_INTERNAL::LowLevelRenderer::Clear(true, true);

			Mesh* mesh = m_ScreenQuadModel->GetMeshes()[0];
			glBindVertexArray(mesh->GetVertexArrayObjectID());
			glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVertexBufferObjectID());
			glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh->GetVertexCount());
			glFlush();
		}
		m_UpscaleTargetMips[0]->Bind(ERenderingMapSlot::BloomOutput);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	 
}
