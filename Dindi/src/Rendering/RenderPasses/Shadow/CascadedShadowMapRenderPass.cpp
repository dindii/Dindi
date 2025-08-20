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

#include <time.h>
#include <vector>

#include <random>

static std::vector<glm::vec4> frustumCorners;

namespace Dindi
{
	CSMRenderPass::CSMRenderPass()
	{
		Application& app = Application::GetInstance();
		glm::vec2 windowDims = app.GetWindow()->GetDimensions();

		m_Framebuffer = new Framebuffer();
		m_Shader = Shader::Load(RESOURCES_PATH "Resources/Shaders/Shadow/SimpleShadowVert.shader", RESOURCES_PATH "Resources/Shaders/Shadow/SimpleShadowFrag.shader");

		Camera* camera = Application::GetInstance().GetActiveScene()->GetActiveCamera();

		float FOV = camera->GetFieldOfView();
		float AR  = camera->GetAspectRatio();

		GraphicsDefinitions& gd = Renderer::GetGraphicsDefinitions();

		m_CSMLightOrthographicViewTransform.resize(gd.NumberOfShadowCascades);

		frustumCorners.resize(2 * 2 * 2 * gd.NumberOfShadowCascades);


		m_CSMPerspectiveProjections.push_back(glm::perspective(FOV, AR, 1.0f, gd.CSMCascadeDistance[0]));
		
		for (uint32_t i = 1; i < gd.NumberOfShadowCascades; i++)
			m_CSMPerspectiveProjections.push_back(glm::perspective(FOV, AR, gd.CSMCascadeDistance[0], gd.CSMCascadeDistance[i]));


		m_CSMTextures.reserve(gd.NumberOfShadowCascades);

		float sizes[3] = { 4096.0f, 1024.0f, 512.0f};

		for (uint32_t i = 0; i < gd.NumberOfShadowCascades; i++)
		{
			RenderTargetDescriptor desc;
			desc.borderColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			desc.width = (uint16_t)sizes[i];
			desc.height = (uint16_t)sizes[i];
			desc.internalFormat = RenderTargetInternalFormat::DND_DETPH_32;
			desc.magFilter = RenderTargetMagMinFilter::NEAREST;
			desc.minFilter = RenderTargetMagMinFilter::NEAREST;
			desc.type = RenderTargetDataType::DND_UNSIGNED_BYTE;
			desc.wrapU = RenderTargetWrapMode::CLAMP_BORDER;
			desc.wrapV = RenderTargetWrapMode::CLAMP_BORDER;

			Texture2D* shadowMap = new Texture2D(desc);
			m_CSMTextures.push_back(shadowMap);
		}

		GenerateRandom3DTexture(16, 8);
		
	}

	float jitter()
	{
		static std::default_random_engine generator; 
		static std::uniform_real_distribution<float> distrib(-0.5f, 0.5f);
		return distrib(generator);

	}

	void CSMRenderPass::GenerateRandom3DTexture(int windowSize, int filterSize)
	{
		int bufferSize = windowSize * windowSize * filterSize * filterSize * 2;
		std::vector<float> data;
		data.resize(bufferSize);

		int index = 0;

		for (int TexY = 0; TexY < windowSize; TexY++)
		{
			for (int TexX = 0; TexX < windowSize; TexX++)
			{
				for (int v = filterSize - 1; v >= 0; v--)
				{
					for (int u = 0; u < filterSize; u++)
					{
						float x = ((float)u + 0.5f + jitter()) / (float)filterSize;
						float y = ((float)v + 0.5f + jitter()) / (float)filterSize;

						data[index] = sqrtf(y) * cosf(2 * glm::pi<float>() * x);
						data[index + 1] = sqrtf(y) * sinf(2 * glm::pi<float>() * x);

						index += 2;
					}
				}
			}
		}


		int NumFilterSamples = filterSize * filterSize;

		glGenTextures(1, &RandomAnglesTexture);
		glBindTexture(GL_TEXTURE_3D, RandomAnglesTexture);
		glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, NumFilterSamples / 2, windowSize, windowSize);
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, NumFilterSamples / 2, windowSize, windowSize, GL_RGBA, GL_FLOAT, &data[0]);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glBindTexture(GL_TEXTURE_3D, 0);
	}

	CSMRenderPass::~CSMRenderPass()
	{

	}

	void CSMRenderPass::GenerateOutput(Scene* scene)
	{
		m_Framebuffer->Bind();
		m_Shader->Bind();

		//Set the viewport to whatever your current render target is
		Application& app = Application::GetInstance();
		glm::vec2 dims = app.GetWindow()->GetDimensions();

		UpdateFrustumCorners();
		RecalculateProjectionMatrix();
		
	//	Renderer::SetCullingType(CullingFaceMode::FRONT);
		TransformAndDraw(scene);
//		Renderer::SetCullingType(CullingFaceMode::BACK);

		m_Framebuffer->UnBind();
		
		 Renderer::SetViewport(0, 0, dims.x, dims.y);
	}

	void CSMRenderPass::RecalculateProjectionMatrix()
	{
		Application& app = Dindi::Application::GetInstance();
		glm::vec2 windowDims = app.GetWindow()->GetDimensions();

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

			//{
			float multiplier = gd.CSMFarPlaneMultiplier[i];
			float radius = (frustumCorners[0 + (i * 8)] - frustumCorners[6 + (i * 8)]).length() / 2.0f;
			//float texelsPerUnit = (radius * 2.0f) / m_CSMTextures[2]->GetWidth();
			float texelsPerUnit = (radius * 2.0f) / 128;
			texelsPerUnit *= multiplier;
			glm::mat4 correction = glm::lookAt(glm::vec3(0.0f), lightDir, { 0.0f, 1.0f, 0.0f }) * glm::scale(glm::mat4(1.0f), glm::vec3(texelsPerUnit));
			glm::mat4 invCorrection = glm::inverse(correction);

			frustumCenter = camera->GetCameraPos();
			frustumCenter = correction * glm::vec4(frustumCenter, 1.0f);
			frustumCenter.x = glm::floor(frustumCenter.x);
			frustumCenter.y = glm::floor(frustumCenter.y);
			frustumCenter = invCorrection * glm::vec4(frustumCenter, 1.0f);
			//}

		//	glm::vec3 eye = frustumCenter + (lightDir * radius * 2.0f);

		//	glm::mat4 lightView = glm::lookAt(eye, frustumCenter, { 0.0f, 1.0f, 0.0f });
			glm::mat4 lightView = glm::lookAt(frustumCenter + lightDir, frustumCenter, { 0.0f, 1.0f, 0.0f });


			m_CSMLightOrthographicViewTransform[i] = glm::ortho(-radius * multiplier, radius * multiplier, -radius * multiplier, radius * multiplier, -radius * multiplier * 5, radius * multiplier * 5) * lightView;
		}
	}

	void CSMRenderPass::TransformAndDraw(Scene* scene)
	{
		GraphicsDefinitions& gd = Renderer::GetGraphicsDefinitions();

		for (uint32_t i = 0; i < gd.NumberOfShadowCascades; i++)
		{
			m_Framebuffer->AttachRenderTarget(*m_CSMTextures[i], FramebufferRenderTargetSlot::DEPTH);
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
					m_Shader->UploadUniformMat4("u_Transform", meshTransform);

					m_Shader->UploadUniformMat4("u_CascadedVP", m_CSMLightOrthographicViewTransform[i]);

					//#TODO: elements to draw.
					Renderer::SetViewport(0, 0, m_CSMTextures[i]->GetWidth(), m_CSMTextures[i]->GetHeight());
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

					//	frustumCorners.emplace_back(worldCoords.x / worldCoords.w, worldCoords.y / worldCoords.w, worldCoords.z / worldCoords.w, 1.0f);
						frustumCorners.emplace_back(worldCoords.x , worldCoords.y, worldCoords.z , 1.0f);
					}
				}
			}
		}
	}


}