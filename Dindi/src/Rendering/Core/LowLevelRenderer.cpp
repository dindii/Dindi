#include "Dindipch.h"
#include "LowLevelRenderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Utils/Logger.h>
#include <Core/Application.h>
#include <Platform/Window.h>
#include <Rendering/Debug/DebugRenderer.h>

namespace Dindi
{
	namespace DND_INTERNAL
	{
		Framebuffer* LowLevelRenderer::m_ScreenOutput = nullptr;

		//We are going to use only one UBO, so this doesn't need to be dynamic.
		//This is static because the user don't need to mess with UBOs, he will not need and doesn't care either. This is internal for us.
		static constexpr uint32_t ConstantBufferSlot = 1;

		ConstantBuffer PersistentData;

		LowLevelRenderer::LowLevelRenderer()
		{
			
		}

		LowLevelRenderer::~LowLevelRenderer()
		{
			delete m_ScreenOutput;
		}

		void LowLevelRenderer::Init()
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
				DND_LOG_FATAL("Failed to initialize GLAD");

			Application& app = Application::GetInstance();

			uint32_t width  = app.GetWindow()->GetWidth();
			uint32_t height = app.GetWindow()->GetHeight();

			SetViewport(0, 0, width, height);
			
			SetClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			//Backface culling.
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			//#TODO: This AA is temporary, I will remove this once we have deferred.
			glEnable(GL_MULTISAMPLE);

			glGenBuffers(1, &PersistentData.handle);
			glBindBuffer(GL_UNIFORM_BUFFER, PersistentData.handle);

			//#NOTE: I got lazy and used sizeof for almost everything related to UBO, so take this into account when adding stuff to the constant buffer.
			//Just use static arrays or use the size of the vector * type when doing calculations.
			glBufferData(GL_UNIFORM_BUFFER, sizeof(PersistentData.data), NULL, GL_STATIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, ConstantBufferSlot, PersistentData.handle);

#ifdef DINDI_DEBUG
			Debug::DebugRenderer::Init();
#endif

			m_ScreenOutput = new Framebuffer();
		}

		void LowLevelRenderer::Draw(Scene* scene)
		{
			Application& app = Application::GetInstance();


			///////////////////////////////////////////// Persistent data (projection, camera, lights etc...)

			Camera* camera = scene->GetActiveCamera();
			mat4 viewProjectionMatrix = camera->GetProjection() * camera->GetViewMatrix();

			PersistentData.data.c_ViewProjection = viewProjectionMatrix;
			PersistentData.data.c_Time = app.GetTime();

			vec3 cameraPos = camera->GetCameraPos();
			PersistentData.data.c_CameraPos = vec4(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);

			LightManager* lightManager = scene->GetLightManager();
			std::vector<GPUPointLightData>& lightsData = lightManager->GetLightsData();
			uint32_t nLights = lightsData.size();

			memcpy(&PersistentData.data.c_Lights, lightsData.data(), sizeof(GPUPointLightData) * nLights);

			PersistentData.data.numLights = nLights;
			
			glBindBuffer(GL_UNIFORM_BUFFER, PersistentData.handle);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PersistentData.data), &PersistentData.data);

			m_ScreenOutput->Bind();
			Clear();

			///////////////////////////////////////////// Persistent data (projection, camera, lights etc...)


			///////////////////////////////////////////// Transform and Draw
			for (uint32_t x = 0; x < scene->GetEntities().size(); x++)
			{
				Model* model = scene->GetEntities()[x];
				
				bool rebuildMeshAABB = false;

				Pickable* baseModel = static_cast<Pickable*>(model);

				if (baseModel->GetPickableDirty())
				{
					model->BuildAABB();
					model->SetPickableDirty(false);
					rebuildMeshAABB = true;
				}

				for (uint32_t y = 0; y < scene->GetEntities()[x]->GetMeshes().size(); y++)
				{
					Mesh* mesh = model->GetMeshes()[y];

					glBindVertexArray(mesh->GetVertexArrayObjectID());
					glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVertexBufferObjectID());

					Material* material = mesh->GetMaterial();
					material->Bind();
					
					/* #NOTE #TODO This needs to be rebuilt. We will rebuilt this using proper transformation hierarchy */
					// {

					//#NOTE: For now, MESH individual rotation will be disabled. In the future, a proper transformation hierarchy will be implemented and this
					//will be addressed.
					vec3 modelRotation = model->GetRotation();

					mat4 rotationTransform  = mat4::Rotate(modelRotation.z, { 0.0f, 0.0f, 1.0f });
					rotationTransform	   *= mat4::Rotate(modelRotation.y, { 0.0f, 1.0f, 0.0f });
					rotationTransform      *= mat4::Rotate(modelRotation.x, { 1.0f, 0.0f, 0.0f });
					
					mat4 translationTransform = mat4::Translate(mesh->GetPosition() + model->GetPosition());

					mat4 modelTransform;
				    modelTransform = translationTransform * rotationTransform * mat4::Scale({ model->GetScale() });
					// }

					//Cache transform
					//#NOTE: This cache is wrong. It is caching the mesh pos and rot into the model. So the cache and the mesh doesn't match.
					//#TODO: Cache it right.
					model->SetTransform(modelTransform);

					//We will only rebuild the AABB if it was changed.
					Pickable* baseMesh = static_cast<Pickable*>(mesh);
					if (baseMesh->GetPickableDirty() || rebuildMeshAABB)
					{
						baseMesh->SetPickableDirty(false);
						AABB meshWorldAABB = mesh->GetOffsetAABB(mesh->GetPosition() + model->GetPosition(), model->GetScale(), model->GetRotation());
						mesh->SetWorldAABB(meshWorldAABB);
					}

					material->GetShader()->UploadUniformMat4("u_Transform", modelTransform);

					//#TODO: elements to draw.
					glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());
				}
			}
			///////////////////////////////////////////// Transform and Draw

			//Draw all debug shapes in the queue
			Debug::DebugRenderer::SubmitDraw();

            //DEBUG RENDERER CALLS ---------------------------------------------------------------------------------------------------------------------------
			//Draw cubes in light positions to debug.
			std::vector<PointLight>& lights = lightManager->GetLights();
			
			if(app.GetApplicationState() == EApplicationState::EDITOR)
				for (uint32_t x = 0; x < lights.size(); x++)
				{
					Debug::DebugShapeContext shapeContext;

					shapeContext.firstPosition = lights[x].GetPosition();
					shapeContext.shapeColor = lights[x].GetColor();
					shapeContext.shapeSize = 0.5f;
					shapeContext.shapeRenderFlags = (Debug::EDebugRenderFlags::WIREFRAME) | (Debug::EDebugRenderFlags::NO_DEPTH_TESTING);
					shapeContext.shapeType = Debug::EDebugShape::CUBE;

					Debug::DebugRenderer::DrawShape(shapeContext);
				}

			m_ScreenOutput->UnBind();
		}

		void LowLevelRenderer::Clear(const bool ColorBuffer /*= true*/, const bool DepthBuffer /*= true*/)
		{
			if (ColorBuffer)
				glClear(GL_COLOR_BUFFER_BIT);

			if (DepthBuffer)
				glClear(GL_DEPTH_BUFFER_BIT);
		}

		void LowLevelRenderer::SetClearColor(const vec4& color)
		{
			glClearColor(color.r, color.g, color.b, color.a);
		}

		void LowLevelRenderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			glViewport(x, y, width, height);
		}

		unsigned int LowLevelRenderer::GenMesh(unsigned int nMeshes /*= 1*/)
		{
			unsigned int VBO = 0;
			glGenBuffers(nMeshes, &VBO);

			return VBO;
		}

		void LowLevelRenderer::SetWireframeMode(bool cond)
		{
			if (cond)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		void LowLevelRenderer::DelMesh(unsigned int Mesh, unsigned int nMeshes /*= 1*/)
		{
			if (Mesh > 0)
			{
				glDeleteBuffers(nMeshes, &Mesh);
			}
		}

		void LowLevelRenderer::RemakeFramebuffers(uint32_t width, uint32_t height)
		{
			m_ScreenOutput->Remake();
		}

		void LowLevelRenderer::SetOverlay(bool cond)
		{
			if (cond)
				glDepthFunc(GL_ALWAYS);
			else
				glDepthFunc(GL_LESS);
		}

	}
}