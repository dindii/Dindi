#include "Dindipch.h"
#include <Rendering/Core/Common.hpp>
#include "LowLevelRenderer.h"
#include <GLFW/glfw3.h>
#include <Utils/Logger.h>
#include <Core/Application.h>
#include <Platform/Window.h>
#include <Rendering/Debug/DebugRenderer.h>
#include <Utils/AssetManager.h>
#include <Rendering/Shader/Shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Dindi
{

	GraphicsDefinitions::GraphicsDefinitions()
	{
		// ----------------------------- Shadow
		CSMFarPlaneMultiplier = 30.0f;

		CSMFarPlaneThresholds.push_back(30.0f);
		CSMFarPlaneThresholds.push_back(80.0f);
		CSMFarPlaneThresholds.push_back(120.0f);
		
		CSMCascadeDistance.push_back(28.0f);
		CSMCascadeDistance.push_back(35.0f);
		CSMCascadeDistance.push_back(140.0f);

		NumberOfShadowCascades = CSMFarPlaneThresholds.size();
		// ----------------------------- Shadow




		// ----------------------------- Light
			directionalLightDir = { 1.0f, 10.0f, 1.0f, 1.0f };
		// ----------------------------- Light
	}

	namespace DND_INTERNAL
	{
		Framebuffer* LowLevelRenderer::m_ScreenOutput = nullptr;
		CSMRenderPass* LowLevelRenderer::m_CSMRenderPass = nullptr;

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

		void glerrorcallback(GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam)
		{
			std::ostringstream os;
			os << "GL LOG: type = " << type << ", severity = " << severity << ", message = " << message;
			DND_LOG_WARNING(os.str());
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

			uint32_t nwidth = app.GetWindow()->GetWidth();
			uint32_t nheight = app.GetWindow()->GetHeight();

			//------ Default screen output
			RenderTargetDescriptor colorDescriptor;
			colorDescriptor.internalFormat = RenderTargetInternalFormat::DND_RGB8;
			colorDescriptor.type = RenderTargetDataType::DND_UNSIGNED_BYTE;
			
			RenderTargetDescriptor depthDescriptor;
			depthDescriptor.internalFormat = RenderTargetInternalFormat::DND_DETPH_UNSIZED;
			depthDescriptor.type = DND_FLOAT;
			

			m_ScreenOutput = new Framebuffer(nwidth, nheight, colorDescriptor, depthDescriptor);
			//------ Default screen output

			glDebugMessageCallback(glerrorcallback, nullptr);
		}

		void LowLevelRenderer::OnAppInitialized()
		{
			m_CSMRenderPass = new CSMRenderPass();
		}

		void LowLevelRenderer::SetConstantData(Scene* scene)
		{
			///////////////////////////////////////////// Persistent data (projection, camera, lights etc...)
			Application& app = Application::GetInstance();

			Camera* camera = scene->GetActiveCamera();
			glm::vec3 cameraPos = camera->GetCameraPos();
			
			glm::mat4 viewProjectionMatrix = camera->GetProjection() * camera->GetViewMatrix();
			PersistentData.data.c_ViewProjection = viewProjectionMatrix;

			const GraphicsDefinitions& gd = m_GraphicsDefinitions;		
			

			PersistentData.data.c_View = camera->GetViewMatrix();
			PersistentData.data.c_DirLightPos = gd.directionalLightDir;
			
			PersistentData.data.c_Time = app.GetTime();

			PersistentData.data.c_CameraPos = glm::vec4(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);

			LightManager* lightManager = scene->GetLightManager();
			std::vector<GPUPointLightData>& lightsData = lightManager->GetLightsData();
			uint32_t nLights = lightsData.size();

			memcpy(&PersistentData.data.c_Lights, lightsData.data(), sizeof(GPUPointLightData) * nLights);

			PersistentData.data.numLights = nLights;

			glBindBuffer(GL_UNIFORM_BUFFER, PersistentData.handle);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PersistentData.data), &PersistentData.data);
			///////////////////////////////////////////// Persistent data (projection, camera, lights etc...)
		}

		void LowLevelRenderer::Draw(Scene* scene)
		{
			SetConstantData(scene);
			ApplySceneTransformation(scene);

			m_CSMRenderPass->GenerateOutput(scene);

			m_ScreenOutput->Bind();
			Clear(true, true);
			OutputPass(scene);
			DebugDraw(scene);
			m_ScreenOutput->UnBind();
		}

		void LowLevelRenderer::OutputPass(Scene* scene)
		{
			const GraphicsDefinitions& gd = m_GraphicsDefinitions;

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

					
					std::vector<glm::mat4>& transforms = m_CSMRenderPass->GetTransforms();
					for (uint32_t i = 0; i < m_GraphicsDefinitions.NumberOfShadowCascades; i++)
					{
						char lightTransformIndex[128];

						sprintf(lightTransformIndex, "u_SingleLightTransform[%i]", i);
						material->GetShader()->UploadUniformMat4(lightTransformIndex, transforms[i]);
						
						memset(lightTransformIndex, 0, 128);
						sprintf(lightTransformIndex, "u_CSMDistances[%i]", i);
						material->GetShader()->UploadUniformFloat(lightTransformIndex, gd.CSMCascadeDistance[i]);
					}
				

					//#TODO: elements to draw.
					glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());
				}
			}
		}

		
		void LowLevelRenderer::ApplySceneTransformation(Scene* scene)
		{
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

					/* #NOTE #TODO This needs to be rebuilt. We will rebuilt this using proper transformation hierarchy */
					// {

					//#NOTE: For now, MESH individual rotation will be disabled. In the future, a proper transformation hierarchy will be implemented and this
					//will be addressed.
					glm::vec3 modelRotation = model->GetRotation();

					glm::mat4 rotationTransform(1.0f);
				    rotationTransform = glm::rotate(rotationTransform, modelRotation.z, { 0.0f, 0.0f, 1.0f });
					rotationTransform = glm::rotate(rotationTransform, modelRotation.y, { 0.0f, 1.0f, 0.0f });
					rotationTransform = glm::rotate(rotationTransform, modelRotation.x, { 1.0f, 0.0f, 0.0f });

					glm::mat4 translationTransform(1.0f);
					//translationTransform = glm::translate(translationTransform, /*mesh->GetPosition()*/ model->GetPosition());
					translationTransform = glm::translate(translationTransform, model->GetPosition() + mesh->GetPosition());

					glm::mat4 modelTransform(1.0f);
					modelTransform = translationTransform * rotationTransform * glm::scale(modelTransform, glm::vec3(model->GetScale()));
					// }

					mesh->SetTransform(modelTransform);

					//We will only rebuild the AABB if it was changed.
					Pickable* baseMesh = static_cast<Pickable*>(mesh);
					if (baseMesh->GetPickableDirty() || rebuildMeshAABB)
					{
						model->BuildAABB();
						baseMesh->SetPickableDirty(false);
						AABB meshWorldAABB = mesh->GetOffsetAABB(mesh->GetPosition() + model->GetPosition(), glm::vec3(model->GetScale()), model->GetRotation());
						mesh->SetWorldAABB(meshWorldAABB);
					}
				}
			}
			///////////////////////////////////////////// Transform and Draw
		}

		void LowLevelRenderer::DebugDraw(Scene* scene)
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
					shapeContext.shapeRenderFlags = (Debug::EDebugRenderFlags::WIREFRAME) | (Debug::EDebugRenderFlags::NO_DEPTH_TESTING);
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

		void LowLevelRenderer::Clear(const bool ColorBuffer /*= true*/, const bool DepthBuffer /*= true*/)
		{
			if (ColorBuffer)
				glClear(GL_COLOR_BUFFER_BIT);

			if (DepthBuffer)
				glClear(GL_DEPTH_BUFFER_BIT);
		}


		void LowLevelRenderer::SetClearColor(const glm::vec4& color)
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

		void LowLevelRenderer::SetCullingType(CullingFaceMode mode)
		{
			mode == CullingFaceMode::FRONT ? glCullFace(GL_FRONT) : glCullFace(GL_BACK);
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