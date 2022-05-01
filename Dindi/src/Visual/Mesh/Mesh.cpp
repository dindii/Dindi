#include "Dindipch.h"
#include "Mesh.h"

namespace Dindi
{
	Mesh::Mesh(const std::vector<vec3>& vertexPositions)
	{
		//#TODO: Convert this to shared ptr, maybe.
		m_VertexPositions = vertexPositions;

		RegisterMeshData(m_VertexPositions);
	}
}