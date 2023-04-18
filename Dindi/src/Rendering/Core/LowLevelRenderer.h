#pragma once

#include "Math/vec4.h"
#include "Rendering/Scene/Scene.h"
#include "Visual/Light/Light.h"

#include <Rendering/Core/Framebuffer.h>

namespace Dindi
{

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
				mat4 c_ViewProjection;
				vec4 c_CameraPos;
				PointLight c_Lights[DND_MAX_LIGHTS];
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
			static void Draw(Scene* scene);

			static void Clear(const bool ColorBuffer = true, const bool DepthBuffer = true);
			static void SetClearColor(const vec4& color);
			static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
			static unsigned int GenMesh(unsigned int nMeshes = 1);
			static void SetWireframeMode(bool cond);
			static void SetOverlay(bool cond);
			static void DelMesh(unsigned int Mesh, unsigned int nMeshes = 1);		
			static void RemakeFramebuffers(uint32_t width, uint32_t height);

			static inline uint32_t GetScreenOutputHandle() { return m_ScreenOutput->GetOutputImage(); }

		private:
			static Framebuffer* m_ScreenOutput;
		};
	}

}