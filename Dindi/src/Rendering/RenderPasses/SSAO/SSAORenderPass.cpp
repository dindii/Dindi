#include <Dindipch.h>
#include "SSAORenderPass.h"
#include <random>
#include "Math/Maths.h"
#include <Core/Application.h>
#include <Platform/Window.h>
#include <Rendering/Core/LowLevelRenderer.h>
#include <Utils/MeshUtils.h>

namespace Dindi
{
	static uint32_t s_DummyVAO = 0;

	SSAORenderPass::SSAORenderPass()
	{
		GenKernelTexture(64);
		GenNoiseTexture(16);

		glGenVertexArrays(1, &s_DummyVAO);

		Application& app = Application::GetInstance();

		uint32_t nwidth = app.GetWindow()->GetWidth();
		uint32_t nheight = app.GetWindow()->GetHeight();

		RenderTargetDescriptor ssaoRTDesc;
		ssaoRTDesc.height = nheight;
		ssaoRTDesc.width = nwidth;

		ssaoRTDesc.internalFormat = RenderTargetInternalFormat::DND_R8;
		ssaoRTDesc.type = RenderTargetDataType::DND_FLOAT;
		ssaoRTDesc.minFilter = RenderTargetMagMinFilter::NEAREST;
		ssaoRTDesc.magFilter = RenderTargetMagMinFilter::NEAREST;
		ssaoRTDesc.wrapU = RenderTargetWrapMode::CLAMP_BORDER;
		ssaoRTDesc.wrapV = RenderTargetWrapMode::CLAMP_BORDER;

		m_SSAOTexture = new Texture2D(ssaoRTDesc);
		m_GBufferQuadModel = MeshUtil::GenerateQuad(RESOURCES_PATH "Resources/Shaders/PostProcessing/SSAO/SSAO.vert", RESOURCES_PATH "Resources/Shaders/PostProcessing/SSAO/SSAO.frag");
		m_Framebuffer = new Framebuffer();
	}

	SSAORenderPass::~SSAORenderPass()
	{

	}

	void SSAORenderPass::GenerateOutput(Scene* scene)
	{
		glFlush();
		Mesh* fullscreenQuadMesh = m_GBufferQuadModel->GetMeshes()[0];
		glBindVertexArray(fullscreenQuadMesh->GetVertexArrayObjectID());
		glBindBuffer(GL_ARRAY_BUFFER, fullscreenQuadMesh->GetVertexBufferObjectID());

		m_Framebuffer->Bind();
		Ref<Shader> ssaoShader =  fullscreenQuadMesh->GetMaterial()->GetShader();
		ssaoShader->Bind();
		m_Framebuffer->AttachRenderTarget(*m_SSAOTexture, FramebufferRenderTargetSlot::ALBEDO_SPECULAR);

		DND_INTERNAL::LowLevelRenderer::Clear(true, true);



		ssaoShader->UploadInt("u_GBuffer_Position", ERenderingMapSlot::GBuffer_Position);
		m_CachedGBuffer[FramebufferRenderTargetSlot::POSITION]->Bind(ERenderingMapSlot::GBuffer_Position);

		ssaoShader->UploadInt("u_GBuffer_Normal", ERenderingMapSlot::Gbuffer_Normal);
		m_CachedGBuffer[FramebufferRenderTargetSlot::NORMAL]->Bind(ERenderingMapSlot::Gbuffer_Normal);

		ssaoShader->UploadInt("u_GBuffer_Albedo", ERenderingMapSlot::GBuffer_Albedo);
		m_CachedGBuffer[FramebufferRenderTargetSlot::ALBEDO_SPECULAR]->Bind(ERenderingMapSlot::GBuffer_Albedo);

		ssaoShader->UploadInt("u_GBuffer_Depth", ERenderingMapSlot::GBuffer_Depth);
		m_CachedGBuffer[FramebufferRenderTargetSlot::DEPTH]->Bind(ERenderingMapSlot::GBuffer_Depth);

		ssaoShader->UploadInt("u_SSAONoise", ERenderingMapSlot::SSAONoise);
		m_NoiseTexture->Bind(ERenderingMapSlot::SSAONoise);

		//#note this is so inelegant 
		for (unsigned int i = 0; i < 64; ++i)
			ssaoShader->UploadUniformFloat3("u_Samples[" + std::to_string(i) + "]", m_RandomKernel[i]);

		Application& app = Application::GetInstance();
		glm::mat4 proj = app.GetActiveScene()->GetActiveCamera()->GetProjection();

		ssaoShader->UploadUniformMat4("u_Projection", proj);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, fullscreenQuadMesh->GetVertexCount());
	}

	void SSAORenderPass::TransformAndDraw(Scene* scene)
	{

	}

	void SSAORenderPass::GenKernelTexture(uint8_t sampleCount)
	{
		std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
		std::default_random_engine generator;

		for (uint32_t i = 0; i < sampleCount; i++)
		{
			float x = randomFloats(generator) * 2.0f - 1.0f;
			float y = randomFloats(generator) * 2.0f - 1.0f;
			float z = randomFloats(generator); 

			glm::vec3 sample(x, y, z);

			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			
			float scale = (float)i / (float)sampleCount;
			scale = lerp(0.1f, 1.0f, scale * scale); //get the samples closer to the origin
			sample *= scale;

			m_RandomKernel.push_back(sample);
		}
	}

	void SSAORenderPass::GenNoiseTexture(uint8_t tileSize)
	{
		std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
		std::default_random_engine generator;

		std::vector<glm::vec3> rotNoise;

		for (uint32_t i = 0; i < tileSize; i++)
		{
			float x = randomFloats(generator) * 2.0f - 1.0f;
			float y = randomFloats(generator) * 2.0f - 1.0f;
			float z = 0; //rotation around z axis

			rotNoise.emplace_back( x, y, z );
		}

		

		RenderTargetDescriptor noiseTex;
		noiseTex.internalFormat = RenderTargetInternalFormat::DND_COLOR_RGBA16F;
		noiseTex.width = (int)sqrt(tileSize);
		noiseTex.height = (int)sqrt(tileSize);
		noiseTex.type = RenderTargetDataType::DND_FLOAT;
		noiseTex.magFilter = RenderTargetMagMinFilter::NEAREST;
		noiseTex.minFilter = RenderTargetMagMinFilter::NEAREST;
		noiseTex.wrapU = RenderTargetWrapMode::REPEAT;
		noiseTex.wrapV = RenderTargetWrapMode::REPEAT;

		m_NoiseTexture = new Texture2D(noiseTex);
		m_NoiseTexture->SetData(&rotNoise[0]);
	}

}