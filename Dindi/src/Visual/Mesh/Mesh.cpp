#include "Dindipch.h"
#include "Mesh.h"
#include "Utils/Logger.h"

namespace Dindi
{	
	Mesh::Mesh(std::string vertexPath /*= DEFAULT_VERTEX_SHADER*/, std::string fragmentPath /*= DEFAULT_FRAGMENT_SHADER*/) : m_Material(nullptr)
	{
		m_Material = new Material(vertexPath, fragmentPath);
	}

	Mesh::Mesh(Material* material)
	{
		m_Material = material;
	}

	Mesh::Mesh(std::vector<vec3>&& vertices, std::string vertexPath /*= DEFAULT_VERTEX_SHADER*/, std::string fragmentPath /*= DEFAULT_FRAGMENT_SHADER*/) noexcept
	{
		m_Material = new Material(vertexPath, fragmentPath);
		m_VertexPositions = vertices;
	}
	
	Mesh::~Mesh()
	{
		if (m_Material)
			delete m_Material;
	}
	
	void Mesh::RegisterData()
	{
		RegisterMeshData(m_VertexPositions, m_Normal, m_TextureCoord);
	}
}