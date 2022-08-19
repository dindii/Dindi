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
			Mesh*  debugMesh = new Mesh(std::move(cubeVertices), RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderVert.shader", RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderFrag.shader");
			
			temporaryCubeModel->AddMesh(debugMesh);

			m_DebugShapes[EDebugShape::CUBE] = temporaryCubeModel;
			//CUBE --------------------------------------------------------

		}

		//#TODO: Add render flags here. Like, if it is wireframe, opaque, if we are going to do depth test (to overlay geometry) or not etc.
#ifdef DINDI_DEBUG
		void DebugRenderer::Draw(EDebugShape shape, vec3& pos, vec3& color, float size, uint32_t flags)
		{
			bool wireframeMode = flags & EDebugRenderFlags::WIREFRAME;
			bool overlayMode   = flags & EDebugRenderFlags::NO_DEPTH_TESTING;

			//Usually, each Debug model will only have 01 mesh. This is more for primitives, like spheres, cubes, triangles etc...
			//#NOTE: If you need more complex debug models with more meshes, you can easily expand this with a for loop or so.
			Mesh* mesh = m_DebugShapes[shape]->GetMeshes()[0];

			mesh->GetMaterial()->Bind();
			mesh->GetMaterial()->GetShader()->UploadUniformFloat3("u_Color", color);

			mat4 transform;
			transform *= transform.Translate(pos);
			transform *= transform.Scale(size);
			
			mesh->GetMaterial()->GetShader()->UploadUniformMat4("u_Transform", transform);

			glBindVertexArray(            mesh->GetVertexArrayObjectID());
			glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVertexBufferObjectID());

			
			DND_INTERNAL::LowLevelRenderer::SetWireframeMode(wireframeMode);
			DND_INTERNAL::LowLevelRenderer::SetOverlay(overlayMode);

			glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());

			DND_INTERNAL::LowLevelRenderer::SetWireframeMode(false);
			DND_INTERNAL::LowLevelRenderer::SetOverlay(false);
		}
#endif
	}
}