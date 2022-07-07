#include "Dindipch.h"
#include "Mesh.h"
#include "Utils/Logger.h"


namespace Dindi
{
	void Mesh::RegisterData()
	{
		RegisterMeshData(m_VertexPositions, m_Normal, m_TextureCoord);
	}

	Mesh::Mesh(std::vector<vec3>&& vertices) noexcept
	{
		m_VertexPositions = vertices;
	}
}