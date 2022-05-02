#include "Dindipch.h"
#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION 1
#define TINYOBJLOADER_USE_MAPBOX_EARCUT 1 //more robust triangulation, requires c++11

namespace Dindi
{
	Mesh::Mesh(const std::vector<vec3>& vertexPositions)
	{
		//#TODO: Convert this to shared ptr, maybe.
		m_VertexPositions = vertexPositions;

		RegisterMeshData(m_VertexPositions);
	}
}