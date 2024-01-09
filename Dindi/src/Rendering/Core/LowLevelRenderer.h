#pragma once

#include "Rendering/Scene/Scene.h"
#include "Visual/Light/LightManager.h"
#include <Rendering/Core/Framebuffer.h>
#include <Rendering/RenderPasses/Shadow/CascadedShadowMapRenderPass.h>

class CSMRenderPass;

namespace Dindi
{
	struct GraphicsDefinitions
	{
		//GraphicsDefinitions() : shadowMapNearPlane(34.615f), shadowMapFarPlane(76.525f), shadowFrustrumDims(50.531f), directionalLightDir(0.0f, 73.457f, 11.728f, 28.313f) {};
		GraphicsDefinitions();
	
		// ----------------------------- Shadow
		float cascadeEnds[4];
		// ----------------------------- Shadow
	
		// ----------------------------- Light
		glm::vec4 directionalLightDir = { 1.0f, 10.0f, 1.0f, 1.0f};
		// ----------------------------- Light
	};


	namespace DND_INTERNAL
	{
		//We will use the active scene to fill the constant data like lights, shadows and projections.
		//I have no intent to have more than one ConstantBuffer, so even in another scenes, we will still have place to
		//support multiple lights, shadows and stuff like that. Because of this, I will use only a single handler id throughout the program.
		struct ConstantBuffer
		{
			struct Data
			{
				float c_Time = 0.0f;
				float _p1 = 0, _p2 = 0;
				uint32_t numLights = 0;
				glm::mat4 c_ViewProjection;
				glm::mat4 c_View;
				glm::vec4 c_CameraPos;
				glm::vec4 c_DirLightPos;
				GPUPointLightData c_Lights[DND_MAX_LIGHTS];
			};

			Data data;
			uint32_t handle = 0;
		};

		class LowLevelRenderer
		{
		public:
			LowLevelRenderer();
			~LowLevelRenderer();

			static void Init();
			static void OnAppInitialized();
			static void Draw(Scene* scene);

			static void Clear(const bool ColorBuffer = true, const bool DepthBuffer = true);
			static void SetClearColor(const glm::vec4& color);
			static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
			static unsigned int GenMesh(unsigned int nMeshes = 1);
			static void SetWireframeMode(bool cond);
			static void SetOverlay(bool cond);
			static void DelMesh(unsigned int Mesh, unsigned int nMeshes = 1);		
			static void RemakeFramebuffers(uint32_t width, uint32_t height);

			static inline uint32_t GetScreenOutputHandle() { return m_ScreenOutput->GetOutputColorImage(); }

			static inline std::vector<Texture2D*>& GetShadowMap() { return m_CSMRenderPass->GetRenderTarget(); }
		
			static GraphicsDefinitions& GetGraphicsDefinitions() { return m_GraphicsDefinitions; }

		private:
			static void SetConstantData(Scene* scene);
			static void DebugDraw(Scene* scene);
			static void ApplySceneTransformation(Scene* scene);

			//Render passes
			static void OutputPass(Scene* scene);
			static void ShadowPass(Scene* scene);

			static Framebuffer* m_ScreenOutput;
			static inline Framebuffer* m_ShadowMap = nullptr;

			//@TODO: TO REFACTOR
		private:
			static inline glm::mat4 m_DirectionalLightView = glm::mat4(1.0f);
			static inline glm::mat4 m_DirectionalLightProjection = glm::mat4(1.0f);
			static inline Ref<Shader> m_ShadowShader = nullptr;
			static inline GraphicsDefinitions m_GraphicsDefinitions;
			static inline Texture2D m_ShadowMapTexture;
			static CSMRenderPass* m_CSMRenderPass;
		};
	}
}