#include "Dindipch.h"
#include "GBufferPass.h"

#include <Platform/Window.h>
#include <Core/Application.h>
#include <Rendering/Core/LowLevelRenderer.h>
#include <Rendering/Debug/DebugRenderer.h>

#include <Utils/MeshUtils.h>

namespace Dindi
{
	GBufferRenderPass::GBufferRenderPass()
	{
		Application& app = Application::GetInstance();
		
		uint32_t nwidth = app.GetWindow()->GetWidth();
		uint32_t nheight = app.GetWindow()->GetHeight();


		//Position, Normal and Color descriptor (we will eventually tone map to 8bit)
		RenderTargetDescriptor halfprecisionFloatDesc;
		halfprecisionFloatDesc.internalFormat = RenderTargetInternalFormat::DND_COLOR_RGBA16F;
		halfprecisionFloatDesc.type = RenderTargetDataType::DND_FLOAT;
		halfprecisionFloatDesc.minFilter = RenderTargetMagMinFilter::NEAREST;
		halfprecisionFloatDesc.magFilter = RenderTargetMagMinFilter::NEAREST;
		halfprecisionFloatDesc.width = nwidth;
		halfprecisionFloatDesc.height = nheight;

		//Depth descriptor
		RenderTargetDescriptor depthPassDesc;
		depthPassDesc.internalFormat = RenderTargetInternalFormat::DND_DETPH_UNSIZED;
		depthPassDesc.type = DND_FLOAT;
		depthPassDesc.width = nwidth;
		depthPassDesc.height = nheight;
		depthPassDesc.magFilter = RenderTargetMagMinFilter::NEAREST;
		depthPassDesc.minFilter = RenderTargetMagMinFilter::NEAREST;

		Texture2D* positionBuffer = new Texture2D(halfprecisionFloatDesc);
		Texture2D* normalBuffer = new Texture2D(halfprecisionFloatDesc);
		Texture2D* albedoSpecularBuffer = new Texture2D(halfprecisionFloatDesc);
		Texture2D* depthBuffer = new Texture2D(depthPassDesc);

		m_GBuffer.push_back(positionBuffer);
		m_GBuffer.push_back(normalBuffer);
		m_GBuffer.push_back(albedoSpecularBuffer);
		m_GBuffer.push_back(depthBuffer);

		m_Framebuffer = new Framebuffer(nwidth, nheight, halfprecisionFloatDesc, depthPassDesc);

		m_Framebuffer->Bind();

		m_Framebuffer->AttachRenderTarget(*positionBuffer, FramebufferRenderTargetSlot::POSITION);
		m_Framebuffer->AttachRenderTarget(*normalBuffer, FramebufferRenderTargetSlot::NORMAL);
		m_Framebuffer->AttachRenderTarget(*albedoSpecularBuffer, FramebufferRenderTargetSlot::ALBEDO_SPECULAR);
		m_Framebuffer->AttachRenderTarget(*depthBuffer, FramebufferRenderTargetSlot::DEPTH);

		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT0};
		glDrawBuffers(3, attachments);

		m_Shader = Shader::Load(RESOURCES_PATH "Resources/Shaders/Deferred/GBuffer.vert", RESOURCES_PATH "Resources/Shaders/Deferred/GBuffer.frag");
	}

	GBufferRenderPass::~GBufferRenderPass()
	{

	}

	void GBufferRenderPass::GenerateOutput(Scene* scene)
	{
		m_Framebuffer->Bind();
		DND_INTERNAL::LowLevelRenderer::Clear(true, true);
		
		TransformAndDraw(scene);

#ifdef DINDI_DEBUG
		TransformAndDrawDebugShapes(scene);
#endif

		m_Framebuffer->UnBind();
	}

	void GBufferRenderPass::TransformAndDraw(Scene* scene)
	{
		for (uint32_t x = 0; x < scene->GetEntities().size(); x++)
		{
			Model* model = scene->GetEntities()[x];

			for (uint32_t y = 0; y < scene->GetEntities()[x]->GetMeshes().size(); y++)
			{
				Mesh* mesh = model->GetMeshes()[y];

				glBindVertexArray(mesh->GetVertexArrayObjectID());
				glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVertexBufferObjectID());

				m_Shader->Bind();
				mesh->GetMaterial()->BindTextures(m_Shader);

				glm::mat4 meshTransform = mesh->GetTransform();

				m_Shader->UploadUniformMat4("u_Transform", meshTransform);

				glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());
			}
		}
	}

	void GBufferRenderPass::TransformAndDrawDebugShapes(Scene* scene)
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