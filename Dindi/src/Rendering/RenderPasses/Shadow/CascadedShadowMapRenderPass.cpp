#include "Dindipch.h"
#include "CascadedShadowMapRenderPass.h"
#include <Core/Application.h>
#include <Platform/Window.h>

#include <Rendering/Core/Renderer.h>
#include <Rendering/Shader/Shader.h>

#include <Math/Maths.h>

#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <limits>
#include <algorithm>

#include <Utils/DNDAssert.h>

static std::vector<glm::vec4> frustumCorners;

namespace Dindi
{
	CSMRenderPass::CSMRenderPass()
	{
		Application& app = Application::GetInstance();
		glm::vec2 windowDims = app.GetWindow()->GetDimensions();

		m_CSMFramebuffer = new Framebuffer();

		m_CSMShader = Shader::Load(RESOURCES_PATH "Resources/Shaders/Shadow/SimpleShadowVert.shader", RESOURCES_PATH "Resources/Shaders/Shadow/SimpleShadowFrag.shader");

		Camera* camera = Application::GetInstance().GetActiveScene()->GetActiveCamera();

		float FOV = camera->GetFieldOfView();
		float AR = camera->GetAspectRatio();

		GraphicsDefinitions& gd = Renderer::GetGraphicsDefinitions();

		m_CSMLightOrthographicViewTransform.resize(gd.NumberOfShadowCascades);

		frustumCorners.resize(2 * 2 * 2 * gd.NumberOfShadowCascades);


		for (uint32_t i = 0; i < gd.NumberOfShadowCascades; i++)
			m_CSMPerspectiveProjections.push_back(glm::perspective(FOV, AR, 1.0f, gd.CSMFarPlaneThresholds[i]));


		m_CSMTextures.reserve(gd.NumberOfShadowCascades);

		for (uint32_t i = 0; i < gd.NumberOfShadowCascades; i++)
		{
			RenderTargetDescriptor desc;
			desc.borderColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			desc.width = (uint16_t)windowDims.x;
			desc.height = (uint16_t)windowDims.y;
			desc.internalFormat = RenderTargetInternalFormat::DND_DETPH_UNSIZED;
			desc.magFilter = RenderTargetMagMinFilter::NEAREST;
			desc.minFilter = RenderTargetMagMinFilter::NEAREST;
			desc.type = RenderTargetDataType::DND_UNSIGNED_BYTE;
			desc.wrapU = RenderTargetWrapMode::CLAMP_BORDER;
			desc.wrapV = RenderTargetWrapMode::CLAMP_BORDER;

			Texture2D* shadowMap = new Texture2D(desc);
			m_CSMTextures.push_back(shadowMap);
		}
	}


	CSMRenderPass::~CSMRenderPass()
	{

	}

	void CSMRenderPass::GenerateOutput(Scene* scene)
	{
		m_CSMFramebuffer->Bind();
		m_CSMShader->Bind();

		//Set the viewport to whatever your current render target is
		Application& app = Application::GetInstance();
		glm::vec2 dims = app.GetWindow()->GetDimensions();
		Renderer::SetViewport(0, 0, dims.x, dims.y);

		UpdateFrustumCorners();
		RecalculateProjectionMatrix();
		
		Renderer::SetCullingType(CullingFaceMode::FRONT);
		TransformAndDraw(scene);
		Renderer::SetCullingType(CullingFaceMode::BACK);

		m_CSMFramebuffer->UnBind();
	}

	void CSMRenderPass::RecalculateProjectionMatrix()
	{
		Application& app = Dindi::Application::GetInstance();
		Scene* scene = app.GetActiveScene();
		Camera* camera = scene->GetActiveCamera();

		GraphicsDefinitions& gd = Renderer::GetGraphicsDefinitions();
		glm::vec3 lightDir = { gd.directionalLightDir.x, gd.directionalLightDir.y, gd.directionalLightDir.z };


		for (uint32_t i = 0; i < gd.NumberOfShadowCascades; i++)
		{
			glm::vec3 frustumCenter(0.0f, 0.0f, 0.0f);
			
			for (uint8_t v = 0; v < 8; v++)
				frustumCenter += glm::vec3(frustumCorners[v + (i * 8)]);

			frustumCenter /= 8.0f;

			glm::mat4 lightView = glm::lookAt(frustumCenter + lightDir, frustumCenter, { 0.0f, 1.0f, 0.0f });

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::min();

			for (uint32_t j = 0; j < 8; j++)
			{
				glm::vec4 temp = lightView * frustumCorners[j + (i * 8)];

				minX = std::min(minX, temp.x);
				maxX = std::max(maxX, temp.x);
				minY = std::min(minY, temp.y);
				maxY = std::max(maxY, temp.y);
				minZ = std::min(minZ, temp.z);
				maxZ = std::max(maxZ, temp.z);
			}

			float multiplier = gd.CSMFarPlaneMultiplier;
			
			m_CSMLightOrthographicViewTransform[i] = glm::ortho(minX, maxX, minY, maxY, minZ - (multiplier * (gd.NumberOfShadowCascades - i)), maxZ + (multiplier * (gd.NumberOfShadowCascades - i))) * lightView;
		}

	}

	void CSMRenderPass::TransformAndDraw(Scene* scene)
	{
		GraphicsDefinitions& gd = Renderer::GetGraphicsDefinitions();

		for (uint32_t i = 0; i < gd.NumberOfShadowCascades; i++)
		{
			m_CSMFramebuffer->AttachRenderTarget(*m_CSMTextures[i], FramebufferRenderTargetSlot::DEPTH);
			Renderer::Clear(false, true);

			for (uint32_t x = 0; x < scene->GetEntities().size(); x++)
			{
				Model* model = scene->GetEntities()[x];

				for (uint32_t y = 0; y < scene->GetEntities()[x]->GetMeshes().size(); y++)
				{
					Mesh* mesh = model->GetMeshes()[y];

					glBindVertexArray(mesh->GetVertexArrayObjectID());
					glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVertexBufferObjectID());


					glm::mat4 meshTransform = mesh->GetTransform();
					m_CSMShader->UploadUniformMat4("u_Transform", meshTransform);

					m_CSMShader->UploadUniformMat4("u_CascadedVP", m_CSMLightOrthographicViewTransform[i]);

					//#TODO: elements to draw.
					glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());
				}
			}
		}
	}

	//#NOTE: Ensure this is called before it is used
	void CSMRenderPass::UpdateFrustumCorners()
	{
		frustumCorners.clear();

		Application& app = Application::GetInstance();
		Camera* camera = app.GetActiveScene()->GetActiveCamera();
		glm::mat4 view = camera->GetViewMatrix();
	
		GraphicsDefinitions& gd = Renderer::GetGraphicsDefinitions();

		for (uint32_t i = 0; i < gd.NumberOfShadowCascades; i++)
		{
			glm::mat4 projection = m_CSMPerspectiveProjections[i];
			glm::mat4 NDCToWorld = glm::inverse(projection * view);

			for (unsigned int x = 0; x < 2; ++x)
			{
				for (unsigned int y = 0; y < 2; ++y)
				{
					for (unsigned int z = 0; z < 2; ++z)
					{
						const glm::vec4 worldCoords =
							NDCToWorld * glm::vec4(
								2.0f * x - 1.0f,
								2.0f * y - 1.0f,
								2.0f * z - 1.0f,
								1.0f);

						frustumCorners.emplace_back(worldCoords.x / worldCoords.w, worldCoords.y / worldCoords.w, worldCoords.z / worldCoords.w, 1.0f);
					}
				}
			}
		}
	}


}