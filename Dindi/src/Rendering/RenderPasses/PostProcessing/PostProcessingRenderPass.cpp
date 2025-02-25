#pragma once
#include <Dindipch.h>
#include "PostProcessingRenderPass.h"
#include <Platform/Window.h>
#include <Core/Application.h>
#include <Rendering/Core/LowLevelRenderer.h>

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

	PostProcessingRenderPass::PostProcessingRenderPass()
	{
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

		
		RenderTargetDescriptor postProcessingColorDesc;
		postProcessingColorDesc.internalFormat = RenderTargetInternalFormat::DND_COLOR_RGB8;
		postProcessingColorDesc.type = RenderTargetDataType::DND_UNSIGNED_BYTE;
		
		m_Framebuffer = new Framebuffer(nwidth, nheight, postProcessingColorDesc);
		
		postProcessingColorDesc.width = nwidth;
		postProcessingColorDesc.height = nheight;
		postProcessingColorDesc.magFilter = RenderTargetMagMinFilter::NEAREST;
		postProcessingColorDesc.minFilter = RenderTargetMagMinFilter::NEAREST;
		postProcessingColorDesc.wrapU = RenderTargetWrapMode::CLAMP_BORDER;
		postProcessingColorDesc.wrapV = RenderTargetWrapMode::CLAMP_BORDER;

		m_RenderTarget = new Texture2D(postProcessingColorDesc);
	}

	void PostProcessingRenderPass::GenerateOutput(Scene* scene)
	{
		m_Framebuffer->Bind();
		TransformAndDraw(scene);
		m_Framebuffer->UnBind();
	}

	void PostProcessingRenderPass::TransformAndDraw(Scene* scene)
	{
		m_Framebuffer->AttachRenderTarget(*m_RenderTarget, FramebufferRenderTargetSlot::COLOR);
		DND_INTERNAL::LowLevelRenderer::Clear(true, true);

		Mesh* mesh = m_ScreenQuadModel->GetMeshes()[0];

		glBindVertexArray(mesh->GetVertexArrayObjectID());
		glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVertexBufferObjectID());

		Ref<Shader> shader = mesh->GetMaterial()->GetShader();
		shader->Bind();
		m_CachedRawRender->Bind(ERenderingMapSlot::RawOutput);
		shader->UploadInt("u_HDRBuffer", ERenderingMapSlot::RawOutput);
		
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh->GetVertexCount());
	}

}