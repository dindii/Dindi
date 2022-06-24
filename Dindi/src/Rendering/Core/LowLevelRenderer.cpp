#include "Dindipch.h"
#include "LowLevelRenderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Utils/Logger.h"
#include "Core/Application.h"


namespace Dindi
{

	namespace DND_INTERNAL
	{
		//We are going to use only one UBO, so this doesn't need to be dynamic.
		//This is static because we user don't need to mess with UBOs, he will not need and doesn't care either. This is internal for us.
		static constexpr uint32_t ConstantBufferSlot = 1;

		ConstantBuffer PersistentData;

		LowLevelRenderer::LowLevelRenderer()
		{
			Init();
		}

		LowLevelRenderer::~LowLevelRenderer()
		{
		}

		void LowLevelRenderer::Init()
		{

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
				DND_LOG_FATAL("Failed to initialize GLAD");

			SetViewport(0, 0, 800, 600);
			//debug
			SetClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });

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
			//Just use raw arrays or use the size of the vector * type when doing calculations.
			glBufferData(GL_UNIFORM_BUFFER, sizeof(PersistentData.data), NULL, GL_STATIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, ConstantBufferSlot, PersistentData.handle);
		}

		void LowLevelRenderer::Draw(Scene* scene)
		{
			Application& app = Application::GetInstance();


			//Persistent data (projection, camera, lights etc...)
			Camera* camera = scene->GetActiveCamera();
			mat4 viewProjectionMatrix = camera->GetProjection() * camera->getViewMatrix();

			PersistentData.data.c_ViewProjection = viewProjectionMatrix;
			PersistentData.data.c_Time = app.GetTime();

			vec3 cameraPos = camera->GetCameraPos();
			PersistentData.data.c_CameraPos = vec4(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);

			std::vector<PointLight>& lights = scene->GetLights();
			uint32_t nLights = lights.size();

			memcpy(&PersistentData.data.c_Lights, lights.data(), sizeof(PointLight) * nLights);

			PersistentData.data.numLights = nLights;
			
			glBindBuffer(GL_UNIFORM_BUFFER, PersistentData.handle);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PersistentData.data), &PersistentData.data);


			for (int32_t x = 0; x < scene->GetEntities().size(); x++)
			{
				//#TODO: It would have another For loop here to iterate the meshes of a model

				Model* model = scene->GetEntities()[x];

				glBindVertexArray(model->GetMesh()->GetVertexArrayObjectID());
				glBindBuffer(GL_ARRAY_BUFFER, model->GetMesh()->GetVertexBufferObjectID());

				Material* material = model->GetMaterial();
				material->Bind();
				
				Mesh* mesh = model->GetMesh();
				glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());
			}
		
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

	}
}