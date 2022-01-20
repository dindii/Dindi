#include "boidspch.h"
#include "Renderable.h"

#include "Rendering/Core/LowLevelRenderer.h"

//TEMPORARY
#include <glad/glad.h>
#include "Utils/Logger.h"
namespace Boids
{																								
	Renderable::Renderable() : m_VertexBufferObjectID(0), m_VertexArrayObjectID(0), m_Elements(0)
	{
		glGenVertexArrays(1, &m_VertexArrayObjectID);
		glBindVertexArray(m_VertexArrayObjectID);

		m_VertexBufferObjectID = GLR_INTERNAL::LowLevelRenderer::GenMesh();
	}

	//This is a way to abstract away opengl buffers from the actual mesh data, this may get more complex if we need
	//more data inside vertex attributes
	void Renderable::RegisterMeshData(std::vector<vec3>& vertexPositions)
	{
		m_Elements = vertexPositions.size();

		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferObjectID);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertexPositions.size(), &vertexPositions[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

}