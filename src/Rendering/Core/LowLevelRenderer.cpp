#include "LowLevelRenderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Utils/Logger.h"

namespace GLR
{
	namespace GLR_INTERNAL
	{
	
		LowLevelRenderer::LowLevelRenderer()
		{
			Init();
		}

		LowLevelRenderer::~LowLevelRenderer()
		{
		}

		void LowLevelRenderer::Init()
		{

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
				GLR_LOG_FATAL("Failed to initialize GLAD");

			SetViewport(0, 0, 800, 600);
			//debug
			SetClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });
		}

		void LowLevelRenderer::Draw(Scene* scene)
		{
			Clear();

			for (int32_t x = 0; x < scene->GetEntities().size(); x++)
			{
// 				glBindVertexArray(scene->GetEntities()[x]->GetVertexArrayObjectID());
// 				glBindBuffer(GL_ARRAY_BUFFER, scene->GetEntities()[x]->GetVertexBufferObjectID());
// 				
// 				Shader* shader = (scene->GetEntities())[x]->GetShader();
// 				shader->Bind();
// 
// 				Camera* camera = scene->GetSceneActiveCamera();
// 
// 				mat4 viewProjectionMatrix = camera->GetProjection() * camera->getViewMatrix();
// 
// 				shader->UploadUniformMat4("u_ViewProjection", viewProjectionMatrix);
// 
// 
// 				glDrawArrays(GL_TRIANGLES, 0, scene->GetEntities()[x]->GetElementsCount());

				glBindVertexArray(scene->GetEntities()[x]->GetMesh()->GetVertexArrayObjectID());
				glBindBuffer(GL_ARRAY_BUFFER, scene->GetEntities()[x]->GetMesh()->GetVertexBufferObjectID());

				Shader* shader = (scene->GetEntities())[x]->GetMaterial()->GetShader();
				shader->Bind();

				Camera* camera = scene->GetSceneActiveCamera();

				mat4 viewProjectionMatrix = camera->GetProjection() * camera->getViewMatrix();

				shader->UploadUniformMat4("u_ViewProjection", viewProjectionMatrix);


				glDrawArrays(GL_TRIANGLES, 0, scene->GetEntities()[x]->GetMesh()->GetElementsCount());
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