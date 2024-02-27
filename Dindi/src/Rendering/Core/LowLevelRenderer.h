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
		GraphicsDefinitions();
	
		/// ----------------------------- Shadow
		
		//In some angles for lower CSM levels, we lost information because our near/far plane ratio is too small
		//this variable extends this ratio for lower level layers.
		std::vector<float> CSMFarPlaneMultiplier;

		//The distance of our FarPlane from our NearPlane. Higher = more area, less detail
		std::vector<float> CSMFarPlaneThresholds;

		 //This defines in which distance we will be using which layer. Ex: [0] = 45.0f; as far as the distance of the pixel
		//is lesser than 45.0f, we will be using the first layer (0).
		std::vector<float> CSMCascadeDistance; 

		//Number of CSM Levels we have.
		uint32_t NumberOfShadowCascades;

		/// ----------------------------- Shadow
	
		/// ----------------------------- Light
		glm::vec4 directionalLightDir;
		/// ----------------------------- Light
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
			static void SetCullingType(CullingFaceMode mode);
			static inline uint32_t GetScreenOutputHandle() { return m_ScreenOutput->GetOutputColorImage(); }

			static inline std::vector<Texture2D*>& GetShadowMap() { return m_CSMRenderPass->GetRenderTarget(); }
		
			static inline uint32_t GetDrawCallNumber() { return m_DrawCallNumber; }
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
			
			static uint32_t m_DrawCallNumber;
		};
	}
}