#include "Dindipch.h"
#include "Mesh.h"
#include "Utils/Logger.h"


namespace Dindi
{
	void Mesh::RegisterData()
	{
		RegisterMeshData(m_VertexPositions, m_Normal, m_TextureCoord);
	}
	void Mesh::SetVertexPositionData(std::vector<vec3>&& vertexPosData)
	{
		m_VertexPositions = vertexPosData;
		int test = m_VertexPositions.size();
	}

}