#pragma once

#include <vector>

#include "Math/vec3.h"
#include "Math/vec2.h"
#include "Rendering/Shader/Shader.h"

namespace Dindi
{
	class Renderable
	{
	public:
		//TEMPORARY, BEFORE ABSTRACTION
		Renderable();
	

		unsigned int virtual GetVertexBufferObjectID() const { return m_VertexBufferObjectID; }
		unsigned int virtual GetVertexArrayObjectID()  const { return m_VertexArrayObjectID; }
		unsigned int virtual GetElementsCount() const { return m_Elements; }
	protected:
		void RegisterMeshData(std::vector<vec3>& vertexPositions, std::vector<vec3>& vertexNormals, std::vector<vec2>& textCoords);
	private:
		//this will have a vbo abstraction
		unsigned int m_VertexBufferObjectID, m_VertexArrayObjectID;
		
		// this will be dynamic and not for a triangle
		int m_Elements;
	};
}