#include "Dindipch.h"
#include <Rendering/Core/Common.hpp>
#include "Renderable.h"

#include "Rendering/Core/LowLevelRenderer.h"

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
	//#TODO: Move this to a register function inside LowLevelRenderer with arbitrary num of arguments of arbitrary types.
	void Renderable::RegisterMeshData(std::vector<glm::vec3>& vertexPositions, std::vector<glm::vec3>& vertexNormals, std::vector<glm::vec2>& textCoords, std::vector<glm::vec3>& tangents)
	{
		glBindVertexArray(m_VertexArrayObjectID);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferObjectID);

		size_t bufferSize = sizeof(glm::vec3) * vertexPositions.size() + sizeof(glm::vec3) * vertexNormals.size() + sizeof(glm::vec2) * textCoords.size() + sizeof(glm::vec3) * tangents.size();

		if(m_Flags & RenderFlags::Static)
			glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
		else if(m_Flags & RenderFlags::Stream)
			glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STREAM_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertexPositions.size(), vertexPositions.data());
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertexPositions.size(), (sizeof(glm::vec3) * vertexNormals.size()), vertexNormals.data());
		glBufferSubData(GL_ARRAY_BUFFER, (sizeof(glm::vec3) * vertexPositions.size()) + (sizeof(glm::vec3) * vertexNormals.size()), (sizeof(glm::vec2) * textCoords.size()), textCoords.data());
		glBufferSubData(GL_ARRAY_BUFFER, (sizeof(glm::vec3) * vertexPositions.size()) + (sizeof(glm::vec3) * vertexNormals.size()) + sizeof(glm::vec2) * textCoords.size(), sizeof(glm::vec3) * tangents.size(), tangents.data());

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(glm::vec3) * vertexPositions.size()));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)((sizeof(glm::vec3) * vertexPositions.size()) + (sizeof(glm::vec3) * vertexNormals.size())));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)((sizeof(glm::vec3) * vertexPositions.size()) + (sizeof(glm::vec3) * vertexNormals.size()) + (sizeof(glm::vec2) * textCoords.size())));

	}

}