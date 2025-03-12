#pragma once
#include <Dindipch.h>
#include "RawRenderPass.h"

#include <Platform/Window.h>
#include <Core/Application.h>
#include <Rendering/Core/Renderer.h>
#include <Rendering/Debug/DebugRenderer.h>

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
	}
	
	RawRenderPass::~RawRenderPass()
	{
	
	}
	
	void RawRenderPass::GenerateOutput(Scene* scene)
	{
		m_Framebuffer->Bind();
		DND_INTERNAL::LowLevelRenderer::Clear(true, true);

		TransformAndDraw(scene);
#ifdef DINDI_DEBUG
		TransformAndDrawDebugShapes(scene);
#endif
		m_Framebuffer->UnBind();
	}
	
	void RawRenderPass::TransformAndDraw(Scene* scene)
	{
		const GraphicsDefinitions& gd = DND_INTERNAL::LowLevelRenderer::GetGraphicsDefinitions();
		
		m_Framebuffer->AttachRenderTarget(*m_RenderTarget, FramebufferRenderTargetSlot::COLOR);
		
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
	}

	void RawRenderPass::TransformAndDrawDebugShapes(Scene* scene)
	{
		//Draw all debug shapes in the queue
		Debug::DebugRenderer::SubmitDraw();

		Application& app = Application::GetInstance();

		//DEBUG RENDERER CALLS ---------------------------------------------------------------------------------------------------------------------------
		//Draw cubes in light positions to debug.]
		LightManager* lightManager = scene->GetLightManager();
		std::vector<PointLight>& lights = lightManager->GetLights();

		if (app.GetApplicationState() == EApplicationState::EDITOR && Debug::DebugRenderer::CheckDebugModeFlag(Debug::LIGHT_BOX))
			for (uint32_t x = 0; x < lights.size(); x++)
			{
				Debug::DebugShapeContext shapeContext;

				shapeContext.firstPosition = lights[x].GetPosition();
				shapeContext.shapeColor = lights[x].GetColor();
				shapeContext.shapeSize = 0.5f;
				shapeContext.shapeRenderFlags = 0;// (Debug::EDebugRenderFlags::NO_DEPTH_TESTING);
				shapeContext.shapeType = Debug::EDebugShape::CUBE;

				Debug::DebugRenderer::DrawShape(shapeContext);
			}



		for (uint32_t x = 0; x < scene->GetEntities().size(); x++)
		{
			Model* model = scene->GetEntities()[x];

			for (uint32_t y = 0; y < scene->GetEntities()[x]->GetMeshes().size(); y++)
			{
				Mesh* mesh = scene->GetEntities()[x]->GetMeshes()[y];



				if (Debug::DebugRenderer::CheckDebugModeFlag(Debug::MESH_AABB))
				{
					Debug::DebugShapeContext shapeContext;
					shapeContext.firstPosition = mesh[x].GetAABB().GetMin();
					shapeContext.shapeColor = glm::vec3(0.0f, 0.0f, 1.0f);
					shapeContext.shapeSize = 0.1f;
					shapeContext.shapeRenderFlags = (Debug::EDebugRenderFlags::NO_DEPTH_TESTING);
					shapeContext.shapeType = Debug::EDebugShape::CUBE;

					Debug::DebugShapeContext shapeContext1;
					shapeContext1.firstPosition = mesh[x].GetAABB().GetMax();
					shapeContext1.shapeColor = glm::vec3(1.0f, 0.0f, 0.0f);
					shapeContext1.shapeSize = 0.1f;
					shapeContext1.shapeRenderFlags = (Debug::EDebugRenderFlags::NO_DEPTH_TESTING);
					shapeContext1.shapeType = Debug::EDebugShape::CUBE;

					Debug::DebugRenderer::DrawShape(shapeContext1);
					Debug::DebugRenderer::DrawShape(shapeContext);
				}


				if (Debug::DebugRenderer::CheckDebugModeFlag(Debug::MODEL_AABB))
				{
					Debug::DebugShapeContext model1;
					model1.firstPosition = model[x].GetAABB().GetMin();
					model1.shapeColor = glm::vec3(0.0f, 1.0f, 0.0f);
					model1.shapeSize = 0.1f;
					model1.shapeRenderFlags = (Debug::EDebugRenderFlags::NO_DEPTH_TESTING);
					model1.shapeType = Debug::EDebugShape::CUBE;
					Debug::DebugShapeContext model2;
					model2.firstPosition = model[x].GetAABB().GetMax();
					model2.shapeColor = glm::vec3(0.0f, 1.0f, 1.0f);
					model2.shapeSize = 0.1f;
					model2.shapeRenderFlags = (Debug::EDebugRenderFlags::NO_DEPTH_TESTING);
					model2.shapeType = Debug::EDebugShape::CUBE;
					Debug::DebugRenderer::DrawShape(model1);
					Debug::DebugRenderer::DrawShape(model2);
				}
			}
		}
	}

}
