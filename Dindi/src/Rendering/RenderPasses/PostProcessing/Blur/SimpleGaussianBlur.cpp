#include <Dindipch.h>
#include "SimpleGaussianBlur.h"
#include "Math/Maths.h"
#include <Core/Application.h>
#include <Platform/Window.h>
#include <Rendering/Core/LowLevelRenderer.h>
#include <Utils/MeshUtils.h>

namespace Dindi
{
	static Model* m_FullscreenQuad = nullptr;

	SimpleGaussianBlurRenderPass::SimpleGaussianBlurRenderPass()
	{
		Application& app = Application::GetInstance();

		uint32_t nwidth = app.GetWindow()->GetWidth();
		uint32_t nheight = app.GetWindow()->GetHeight();

		RenderTargetDescriptor blurRTDesc;
		blurRTDesc.height = nheight;
		blurRTDesc.width = nwidth;

		blurRTDesc.internalFormat = RenderTargetInternalFormat::DND_R8;
		blurRTDesc.type = RenderTargetDataType::DND_FLOAT;
		blurRTDesc.minFilter = RenderTargetMagMinFilter::LINEAR;
		blurRTDesc.magFilter = RenderTargetMagMinFilter::LINEAR;
		blurRTDesc.wrapU = RenderTargetWrapMode::CLAMP_BORDER;
		blurRTDesc.wrapV = RenderTargetWrapMode::CLAMP_BORDER;

		m_BlurTexture = new Texture2D(blurRTDesc);
		m_FullscreenQuad = MeshUtil::GenerateQuad(RESOURCES_PATH "Resources/Shaders/PostProcessing/Blur/SimpleGaussianBlur.vert", RESOURCES_PATH "Resources/Shaders/PostProcessing/Blur/SimpleGaussianBlur.frag");
		
		m_Framebuffer = new Framebuffer();
		m_Framebuffer->AttachRenderTarget(*m_BlurTexture, FramebufferRenderTargetSlot::ALBEDO_SPECULAR);
	}

	SimpleGaussianBlurRenderPass::~SimpleGaussianBlurRenderPass()
	{

	}

	void SimpleGaussianBlurRenderPass::GenerateOutput(Scene* scene)
	{
		glFlush();

		Mesh* fullscreenQuadMesh = m_FullscreenQuad->GetMeshes()[0];
		glBindVertexArray(fullscreenQuadMesh->GetVertexArrayObjectID());
		glBindBuffer(GL_ARRAY_BUFFER, fullscreenQuadMesh->GetVertexBufferObjectID());

		m_Framebuffer->Bind();
		Ref<Shader> blurShader = fullscreenQuadMesh->GetMaterial()->GetShader();
		blurShader->Bind();
		m_Framebuffer->AttachRenderTarget(*m_BlurTexture, FramebufferRenderTargetSlot::ALBEDO_SPECULAR);

		DND_INTERNAL::LowLevelRenderer::Clear(true, true);

		blurShader->UploadInt("u_SSAOTexture", ERenderingMapSlot::Util1);
		blurShader->UploadUniformFloat("u_IsVertical", 1.0f);
		m_CachedTextureToBlur->Bind(ERenderingMapSlot::Util1);


		glDrawArrays(GL_TRIANGLE_STRIP, 0, fullscreenQuadMesh->GetVertexCount());

		glFlush();
		
		blurShader->UploadUniformFloat("u_IsVertical", -1.0f);
		m_BlurTexture->Bind(ERenderingMapSlot::Util1);
		m_Framebuffer->AttachRenderTarget(*m_CachedTextureToBlur, FramebufferRenderTargetSlot::ALBEDO_SPECULAR);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, fullscreenQuadMesh->GetVertexCount());

		glFlush();
	}

	void SimpleGaussianBlurRenderPass::TransformAndDraw(Scene* scene)
	{

	}

}