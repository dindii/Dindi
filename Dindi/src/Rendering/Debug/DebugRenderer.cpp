#pragma once
#include "Dindipch.h"
#include "DebugRenderer.h"

#include <glad/glad.h>
namespace Dindi
{
	namespace Debug
	{
		Model* DebugRenderer::m_DebugShapes[DebugShape::MAX];

		void DebugRenderer::Init()
		{
			//CUBE --------------------------------------------------------
			std::vector<vec3> cubeVertices;
			const uint32_t cubeNumVertices = 36;

			cubeVertices.reserve(cubeNumVertices);
			cubeVertices.resize(cubeNumVertices);

			float cubeVerticesArray[cubeNumVertices * 3] = {
				-0.5f, -0.5f, -0.5f,
				 0.5f, -0.5f, -0.5f,
				 0.5f,  0.5f, -0.5f,
				 0.5f,  0.5f, -0.5f,
				-0.5f,  0.5f, -0.5f,
				-0.5f, -0.5f, -0.5f,

				-0.5f, -0.5f,  0.5f,
				 0.5f, -0.5f,  0.5f,
				 0.5f,  0.5f,  0.5f,
				 0.5f,  0.5f,  0.5f,
				-0.5f,  0.5f,  0.5f,
				-0.5f, -0.5f,  0.5f,

				-0.5f,  0.5f,  0.5f,
				-0.5f,  0.5f, -0.5f,
				-0.5f, -0.5f, -0.5f,
				-0.5f, -0.5f, -0.5f,
				-0.5f, -0.5f,  0.5f,
				-0.5f,  0.5f,  0.5f,

				 0.5f,  0.5f,  0.5f,
				 0.5f,  0.5f, -0.5f,
				 0.5f, -0.5f, -0.5f,
				 0.5f, -0.5f, -0.5f,
				 0.5f, -0.5f,  0.5f,
				 0.5f,  0.5f,  0.5f,

				-0.5f, -0.5f, -0.5f,
				 0.5f, -0.5f, -0.5f,
				 0.5f, -0.5f,  0.5f,
				 0.5f, -0.5f,  0.5f,
				-0.5f, -0.5f,  0.5f,
				-0.5f, -0.5f, -0.5f,

				-0.5f,  0.5f, -0.5f,
				 0.5f,  0.5f, -0.5f,
				 0.5f,  0.5f,  0.5f,
				 0.5f,  0.5f,  0.5f,
				-0.5f,  0.5f,  0.5f,
				-0.5f,  0.5f, -0.5f
			};

			memcpy(&cubeVertices[0], cubeVerticesArray, cubeNumVertices * sizeof(vec3));

			m_DebugShapes[DebugShape::CUBE] = new Model(cubeVertices);
			//CUBE --------------------------------------------------------

		}

		//#TODO: Add render flags here. Like, if it is wireframe, opaque, if we are going to do depth test (to overlay geometry) or not etc.
		void DebugRenderer::Draw(DebugShape shape, vec3& pos, vec3& color, float size)
		{
			Mesh* mesh = m_DebugShapes[shape]->GetMesh();

			m_DebugShapes[shape]->GetMaterial()->Bind();

			glBindVertexArray(            mesh->GetVertexArrayObjectID());
			glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVertexBufferObjectID());
			glDrawArrays(GL_TRIANGLES, 0, mesh->GetVertexCount());
		}
	}
}