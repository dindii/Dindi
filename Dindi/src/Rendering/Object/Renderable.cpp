#include "Dindipch.h"
#include "Renderable.h"

#include "Rendering/Core/LowLevelRenderer.h"

//TEMPORARY
#include <glad/glad.h>
#include "Utils/Logger.h"
namespace Dindi
{																								
	Renderable::Renderable() : m_VertexBufferObjectID(0), m_VertexArrayObjectID(0), m_Elements(0)
	{
		glGenVertexArrays(1, &m_VertexArrayObjectID);
		glBindVertexArray(m_VertexArrayObjectID);

		m_VertexBufferObjectID = DND_INTERNAL::LowLevelRenderer::GenMesh();
	}

	//This is a way to abstract away opengl buffers from the actual mesh data, this may get more complex if we need
	//more data inside vertex attributes
	void Renderable::RegisterMeshData(std::vector<vec3>& vertexPositions, std::vector<vec3>& vertexNormals, std::vector<vec2>& textCoords)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferObjectID);

		size_t bufferSize = sizeof(vec3) * vertexPositions.size() + sizeof(vec3) * vertexNormals.size() + sizeof(vec2) * textCoords.size();

		glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
		
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * vertexPositions.size(), vertexPositions.data());
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec3) * vertexPositions.size(), (sizeof(vec3) * vertexNormals.size()), vertexNormals.data());
		glBufferSubData(GL_ARRAY_BUFFER, (sizeof(vec3) * vertexPositions.size()) + (sizeof(vec3) * vertexNormals.size()), (sizeof(vec2) * textCoords.size()), textCoords.data());

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(vec3) * vertexPositions.size()));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)((sizeof(vec3) * vertexPositions.size()) + (sizeof(vec3) * vertexNormals.size())));

	}

}