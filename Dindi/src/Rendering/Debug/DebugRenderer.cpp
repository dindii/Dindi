#pragma once
#include "Dindipch.h"
#include "DebugRenderer.h"
#include "Rendering/Core/LowLevelRenderer.h"
#include <Core/Application.h>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

#ifdef DINDI_DEBUG
namespace Dindi
{
	namespace Debug
	{
		Model* DebugRenderer::m_DebugShapes[EDebugShape::MAX];
		std::vector<DebugShapeContext> DebugRenderer::m_OnFlyDrawCalls;

		void DebugRenderer::Init()
		{
			//CUBE --------------------------------------------------------
			std::vector<glm::vec3> cubeVertices;
			const uint32_t cubeNumVertices = 36;

			cubeVertices.reserve(cubeNumVertices);
			cubeVertices.resize(cubeNumVertices);

			float cubeVerticesArray[cubeNumVertices * 3] = {
				-1.0f,-1.0f,-1.0f, 
				-1.0f,-1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f, 
				 1.0f, 1.0f,-1.0f, 
				-1.0f,-1.0f,-1.0f,
				-1.0f, 1.0f,-1.0f, 
				 1.0f,-1.0f, 1.0f,
				-1.0f,-1.0f,-1.0f,
				 1.0f,-1.0f,-1.0f,
				 1.0f, 1.0f,-1.0f,
				 1.0f,-1.0f,-1.0f,
				-1.0f,-1.0f,-1.0f,
				-1.0f,-1.0f,-1.0f,
				-1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f,-1.0f,
				 1.0f,-1.0f, 1.0f,
				-1.0f,-1.0f, 1.0f,
				-1.0f,-1.0f,-1.0f,
				-1.0f, 1.0f, 1.0f,
				-1.0f,-1.0f, 1.0f,
				 1.0f,-1.0f, 1.0f,
				 1.0f, 1.0f, 1.0f,
				 1.0f,-1.0f,-1.0f,
				 1.0f, 1.0f,-1.0f,
				 1.0f,-1.0f,-1.0f,
				 1.0f, 1.0f, 1.0f,
				 1.0f,-1.0f, 1.0f,
				 1.0f, 1.0f, 1.0f,
				 1.0f, 1.0f,-1.0f,
				-1.0f, 1.0f,-1.0f,
				 1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f,-1.0f,
				-1.0f, 1.0f, 1.0f,
				 1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f,
				 1.0f,-1.0f, 1.0f
			};

			memcpy(&cubeVertices[0], cubeVerticesArray, cubeNumVertices * sizeof(glm::vec3));

			Model* temporaryCubeModel = new Model();
			Mesh*  cubeDebugMesh = new Mesh(std::move(cubeVertices), RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderVert.shader", RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderFrag.shader");
			
			temporaryCubeModel->AddMesh(cubeDebugMesh);

			m_DebugShapes[EDebugShape::CUBE] = temporaryCubeModel;
			//CUBE --------------------------------------------------------

			//LINES -------------------------------------------------------
			std::vector<glm::vec3> lineVertices;
			const uint32_t lineNumVertices = 2;

			lineVertices.reserve(lineNumVertices);
			lineVertices.resize(lineNumVertices);

			float lineVerticesArray[lineNumVertices * 3] = {
				0.0f, 0.0f, 0.0f, 
				0.1f, 0.1f, 0.1f,
			};

			memcpy(&lineVertices[0], lineVerticesArray, lineNumVertices * sizeof(glm::vec3));

			Model* temporaryLineModel = new Model();
			Mesh*  lineDebugMesh = new Mesh(std::move(lineVertices), RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderVert.shader", RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderFrag.shader");

			lineDebugMesh->SetRenderFlag(RenderFlags::Stream);
			temporaryLineModel->AddMesh(lineDebugMesh);

			m_DebugShapes[EDebugShape::LINE] = temporaryLineModel;
			//LINES -------------------------------------------------------

			//#TOREMOVE
			AppendDebugModeFlag(LIGHT_BOX);

		}

		void DebugRenderer::SubmitDraw()
		{
			for (uint32_t i = 0; i < m_OnFlyDrawCalls.size(); i++)
			{
				DebugShapeContext& shapeContext = m_OnFlyDrawCalls[i];
				bool isAlive = shapeContext.Tick();

				if (!isAlive)
				{
					m_OnFlyDrawCalls.erase(m_OnFlyDrawCalls.begin() + i);
					continue;
				}

				ImmediateDebugDrawShape(shapeContext);
			}
		}

		void DebugRenderer::DrawShape(const DebugShapeContext& debugShapeContext)
		{
			Application& app = Application::GetInstance();

			if (!app.GetApplicationState() == EApplicationState::SIMULATION)
			{
				if (debugShapeContext.shapeLifetime > 0)
				{
					m_OnFlyDrawCalls.emplace_back(std::move(debugShapeContext));
					return;
				}
				ImmediateDebugDrawShape(debugShapeContext);
			}
		}


		void DebugRenderer::ClearQueue()
		{
			m_OnFlyDrawCalls.clear();
		}

		//#TODO: Add render flags here. Like, if it is wire frame, opaque, if we are going to do depth test (to overlay geometry) or not etc.
		void DebugRenderer::ImmediateDebugDrawShape(const DebugShapeContext& debugShapeContext)
		{
			bool wireframeMode = debugShapeContext.shapeRenderFlags & EDebugRenderFlags::WIREFRAME;
			bool overlayMode   = debugShapeContext.shapeRenderFlags & EDebugRenderFlags::NO_DEPTH_TESTING;

			bool isLine = (debugShapeContext.shapeType == EDebugShape::LINE);

			//Usually, each Debug model will only have 01 mesh. This is more for primitives, like spheres, cubes, triangles etc...
			//#NOTE: If you need more complex debug models with more meshes, you can easily expand this with a for loop or so.
			Mesh* mesh = m_DebugShapes[debugShapeContext.shapeType]->GetMeshes()[0];

			mesh->GetMaterial()->Bind();
			mesh->GetMaterial()->GetShader()->UploadUniformFloat3("u_Color", debugShapeContext.shapeColor);

			glm::mat4 transform(1.0f);
			transform = glm::translate(transform, debugShapeContext.firstPosition);

			if (isLine)
				glLineWidth((uint32_t)debugShapeContext.shapeSize);
			else
			{
				transform = glm::scale(transform, glm::vec3(debugShapeContext.shapeSize));
				glLineWidth(1.0f);
			}
			
			mesh->GetMaterial()->GetShader()->UploadUniformMat4("u_Transform", transform);

			glBindVertexArray(            mesh->GetVertexArrayObjectID());
			glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVertexBufferObjectID());

			//if line, update the buffer
			
			DND_INTERNAL::LowLevelRenderer::SetWireframeMode(wireframeMode);
			DND_INTERNAL::LowLevelRenderer::SetOverlay(overlayMode);

			//if line, drawarrays using GL_LINE
			if(isLine)
			{
				static std::vector<glm::vec3> linePoints(2);
				linePoints[0] = debugShapeContext.firstPosition;
				linePoints[1] = debugShapeContext.secondPosition;

				mesh->SetVertexPositionData(std::move(linePoints));
				mesh->RegisterData();
				
				glDrawArrays(GL_LINES, 0, mesh->GetVertexCount());
			}
			else
			{
				glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());
			}

			DND_INTERNAL::LowLevelRenderer::SetWireframeMode(false);
			DND_INTERNAL::LowLevelRenderer::SetOverlay(false);
		}

		bool DebugShapeContext::Tick()
		{
			if (m_FirstTick)
			{
				m_FirstTick = false;

				std::chrono::duration now = std::chrono::system_clock::now().time_since_epoch();
				m_AliveUntil = std::chrono::duration_cast<std::chrono::milliseconds>(now).count() + shapeLifetime;
			}

			std::chrono::duration now = std::chrono::system_clock::now().time_since_epoch();
			time_t actualTime = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();

			return (m_AliveUntil >= actualTime);
		}

	}
}
#endif