#include "Dindipch.h"
#include "Mesh.h"
#include "Utils/Logger.h"


namespace Dindi
{
	void Mesh::RegisterData()
	{
		RegisterMeshData(m_VertexPositions, m_Normal, m_TextureCoord);
	}
}