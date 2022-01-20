#include "boidspch.h"
#include "Mesh.h"

namespace Boids
{
	Mesh::Mesh(const std::vector<vec3>& vertexPositions)
	{
		//#TODO: Convert this to shared ptr, maybe.
		m_VertexPositions = vertexPositions;

		RegisterMeshData(m_VertexPositions);
	}
}