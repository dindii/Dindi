#pragma once
#include <Dindipch.h>
#include "RawRenderPass.h"

#include <Platform/Window.h>
#include <Core/Application.h>
#include <Rendering/Core/Renderer.h>
#include <Rendering/Debug/DebugRenderer.h>

#include <Utils/MeshUtils.h>

namespace Dindi
{
	RawRenderPass::RawRenderPass()
	{
		Application& app = Application::GetInstance();

		uint32_t nwidth = app.GetWindow()->GetWidth();
		uint32_t nheight = app.GetWindow()->GetHeight();

		//------ Default screen output
		RenderTargetDescriptor rawPassColorDescriptor;
		rawPassColorDescriptor.internalFormat = RenderTargetInternalFormat::DND_COLOR_RGBA16F;
		rawPassColorDescriptor.type = RenderTargetDataType::DND_FLOAT;

		RenderTargetDescriptor rawPassdepthDescriptor;
		rawPassdepthDescriptor.internalFormat = RenderTargetInternalFormat::DND_DETPH_UNSIZED;
		rawPassdepthDescriptor.type = DND_FLOAT;

		m_Framebuffer = new Framebuffer(nwidth, nheight, rawPassColorDescriptor, rawPassdepthDescriptor);

		rawPassColorDescriptor.width = nwidth;
		rawPassColorDescriptor.height = nheight;
		rawPassColorDescriptor.magFilter = RenderTargetMagMinFilter::NEAREST;
		rawPassColorDescriptor.minFilter = RenderTargetMagMinFilter::NEAREST;
		rawPassColorDescriptor.wrapU = RenderTargetWrapMode::CLAMP_BORDER;
		rawPassColorDescriptor.wrapV = RenderTargetWrapMode::CLAMP_BORDER;

		m_RenderTarget = new Texture2D(rawPassColorDescriptor);
		//------ Default screen output

		m_GBufferQuadModel = MeshUtil::GenerateQuad(RESOURCES_PATH "Resources/Shaders/Deferred/DeferredFinalPass.vert", RESOURCES_PATH "Resources/Shaders/Deferred/DeferredFinalPass.frag");
	}
	
	RawRenderPass::~RawRenderPass()
	{
	
	}
	
	void RawRenderPass::GenerateOutput(Scene* scene)
	{
		m_Framebuffer->Bind();
		DND_INTERNAL::LowLevelRenderer::Clear(true, true);

		TransformAndDraw(scene);

		m_Framebuffer->UnBind();
	}
	
	void RawRenderPass::TransformAndDraw(Scene* scene)
	{
#if 0
		const GraphicsDefinitions& gd = DND_INTERNAL::LowLevelRenderer::GetGraphicsDefinitions();
		
		m_Framebuffer->AttachRenderTarget(*m_RenderTarget, FramebufferRenderTargetSlot::ALBEDO_SPECULAR);
		
		for (uint32_t x = 0; x < scene->GetEntities().size(); x++)
		{
			Model* model = scene->GetEntities()[x];

			for (uint32_t y = 0; y < scene->GetEntities()[x]->GetMeshes().size(); y++)
			{
				Mesh* mesh = model->GetMeshes()[y];



				glBindVertexArray(mesh->GetVertexArrayObjectID());
				glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVertexBufferObjectID());

				Material* material = mesh->GetMaterial();
				material->Bind();

				glm::mat4 meshTransform = mesh->GetTransform();

				material->GetShader()->UploadUniformMat4("u_Transform", meshTransform);


				for (uint32_t i = 0; i < m_CachedShadowTransforms.size(); i++)
				{
					char lightTransformIndex[128];

					sprintf(lightTransformIndex, "u_SingleLightTransform[%i]", i);
					material->GetShader()->UploadUniformMat4(lightTransformIndex, m_CachedShadowTransforms[i]);

					memset(lightTransformIndex, 0, 128);
					sprintf(lightTransformIndex, "u_CSMDistances[%i]", i);
					material->GetShader()->UploadUniformFloat(lightTransformIndex, gd.CSMCascadeDistance[i]);
				}


				glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());
			}
		}
#endif

		//The image rendered using the G-Buffer will be stored here
		m_Framebuffer->AttachRenderTarget(*m_RenderTarget, FramebufferRenderTargetSlot::ALBEDO_SPECULAR);

		const GraphicsDefinitions& gd = DND_INTERNAL::LowLevelRenderer::GetGraphicsDefinitions();

		Mesh* fullscreenQuadMesh = m_GBufferQuadModel->GetMeshes()[0];

		glBindVertexArray(fullscreenQuadMesh->GetVertexArrayObjectID());
		glBindBuffer(GL_ARRAY_BUFFER, fullscreenQuadMesh->GetVertexBufferObjectID());


		Material* deferredPassMainMaterial = fullscreenQuadMesh->GetMaterial();
		deferredPassMainMaterial->Bind();
		
		Ref<Shader> deferredPassMainShader = deferredPassMainMaterial->GetShader();
		deferredPassMainShader->Bind();

		deferredPassMainShader->UploadInt("u_GBuffer_Position", ERenderingMapSlot::GBuffer_Position);
		m_CachedGBuffer[FramebufferRenderTargetSlot::POSITION]->Bind(ERenderingMapSlot::GBuffer_Position);

		deferredPassMainShader->UploadInt("u_GBuffer_Normal", ERenderingMapSlot::Gbuffer_Normal);
		m_CachedGBuffer[FramebufferRenderTargetSlot::NORMAL]->Bind(ERenderingMapSlot::Gbuffer_Normal);

		deferredPassMainShader->UploadInt("u_GBuffer_Albedo", ERenderingMapSlot::GBuffer_Albedo);
		m_CachedGBuffer[FramebufferRenderTargetSlot::ALBEDO_SPECULAR]->Bind(ERenderingMapSlot::GBuffer_Albedo);

		deferredPassMainShader->UploadInt("u_GBuffer_Depth", ERenderingMapSlot::GBuffer_Depth);
		m_CachedGBuffer[FramebufferRenderTargetSlot::DEPTH]->Bind(ERenderingMapSlot::GBuffer_Depth);

		for (uint32_t i = 0; i < m_CachedShadowTransforms.size(); i++)
		{
			char lightTransformIndex[128];

			sprintf(lightTransformIndex, "u_SingleLightTransform[%i]", i);
			deferredPassMainShader->UploadUniformMat4(lightTransformIndex, m_CachedShadowTransforms[i]);

			memset(lightTransformIndex, 0, 128);
			sprintf(lightTransformIndex, "u_CSMDistances[%i]", i);
			deferredPassMainShader->UploadUniformFloat(lightTransformIndex, gd.CSMCascadeDistance[i]);
		}

		glDrawArrays(GL_TRIANGLE_STRIP, 0, fullscreenQuadMesh->GetVertexCount());
	}
}
