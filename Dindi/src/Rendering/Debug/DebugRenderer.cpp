#pragma once
#include "Dindipch.h"
#include "DebugRenderer.h"
#include "Rendering/Core/LowLevelRenderer.h"

#include <glad/glad.h>
namespace Dindi
{
	namespace Debug
	{
		Model* DebugRenderer::m_DebugShapes[EDebugShape::MAX];

		void DebugRenderer::Init()
		{
			//CUBE --------------------------------------------------------
			std::vector<vec3> cubeVertices;
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

			memcpy(&cubeVertices[0], cubeVerticesArray, cubeNumVertices * sizeof(vec3));

			Model* temporaryCubeModel = new Model();
			Mesh*  cubeDebugMesh = new Mesh(std::move(cubeVertices), RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderVert.shader", RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderFrag.shader");
			
			temporaryCubeModel->AddMesh(cubeDebugMesh);

			m_DebugShapes[EDebugShape::CUBE] = temporaryCubeModel;
			//CUBE --------------------------------------------------------

			//LINES -------------------------------------------------------
			std::vector<vec3> lineVertices;
			const uint32_t lineNumVertices = 2;

			lineVertices.reserve(lineNumVertices);
			lineVertices.resize(lineNumVertices);

			float lineVerticesArray[lineNumVertices * 3] = {
				0.0f, 0.0f, 0.0f, 
				0.1f, 0.1f, 0.1f,
			};

			memcpy(&lineVertices[0], lineVerticesArray, lineNumVertices * sizeof(vec3));

			Model* temporaryLineModel = new Model();
			Mesh*  lineDebugMesh = new Mesh(std::move(lineVertices), RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderVert.shader", RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderFrag.shader");

			lineDebugMesh->SetRenderFlag(RenderFlags::Stream);
			temporaryLineModel->AddMesh(lineDebugMesh);

			m_DebugShapes[EDebugShape::LINE] = temporaryLineModel;
			//LINES -------------------------------------------------------



		}

		//#TODO: Add render flags here. Like, if it is wire frame, opaque, if we are going to do depth test (to overlay geometry) or not etc.
#ifdef DINDI_DEBUG
		void DebugRenderer::DrawShape(EDebugShape shape, const vec3& pos, const vec3& secondPos, const vec3& color, float size, uint32_t flags)
		{
			bool wireframeMode = flags & EDebugRenderFlags::WIREFRAME;
			bool overlayMode   = flags & EDebugRenderFlags::NO_DEPTH_TESTING;

			bool isLine = (shape == EDebugShape::LINE);

			//Usually, each Debug model will only have 01 mesh. This is more for primitives, like spheres, cubes, triangles etc...
			//#NOTE: If you need more complex debug models with more meshes, you can easily expand this with a for loop or so.
			Mesh* mesh = m_DebugShapes[shape]->GetMeshes()[0];

			mesh->GetMaterial()->Bind();
			mesh->GetMaterial()->GetShader()->UploadUniformFloat3("u_Color", color);

			mat4 transform;
			transform *= transform.Translate(pos);

			if (isLine)
				glLineWidth((uint32_t)size);
			else
			{
				transform *= transform.Scale(size);
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
				static std::vector<vec3> linePoints(2);
				linePoints[0] = pos;
				linePoints[1] = secondPos;

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
#endif
	}
}