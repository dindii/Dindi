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

			//m_DebugShapes[EDebugShape::CUBE] = new Model(cubeVertices, RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderVert.shader", RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderFrag.shader");
			m_DebugShapes[EDebugShape::CUBE] = new Model(cubeVertices, RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderVert.shader", RESOURCES_PATH "Resources/Shaders/Debug/DebugShaderFrag.shader"); 
			//CUBE --------------------------------------------------------

		}

		//#TODO: Add render flags here. Like, if it is wireframe, opaque, if we are going to do depth test (to overlay geometry) or not etc.
		void DebugRenderer::Draw(EDebugShape shape, vec3& pos, vec3& color, float size, uint32_t flags)
		{
			bool wireframeMode = flags & EDebugRenderFlags::WIREFRAME;
			bool overlayMode   = flags & EDebugRenderFlags::NO_DEPTH_TESTING;

			Mesh* mesh = m_DebugShapes[shape]->GetMesh();

			m_DebugShapes[shape]->GetMaterial()->Bind();
			m_DebugShapes[shape]->GetMaterial()->GetShader()->UploadUniformFloat3("u_Color", color);

			mat4 transform;
			transform *= transform.Translate(pos);
			transform *= transform.Scale(size);
			
			m_DebugShapes[shape]->GetMaterial()->GetShader()->UploadUniformMat4("u_Transform", transform);

			glBindVertexArray(            mesh->GetVertexArrayObjectID());
			glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVertexBufferObjectID());

			
			DND_INTERNAL::LowLevelRenderer::SetWireframeMode(wireframeMode);
			DND_INTERNAL::LowLevelRenderer::SetOverlay(overlayMode);

			glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());

			DND_INTERNAL::LowLevelRenderer::SetWireframeMode(false);
			DND_INTERNAL::LowLevelRenderer::SetOverlay(false);
		}
	}
}