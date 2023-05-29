#include "Dindipch.h"
#include "Mesh.h"
#include "Utils/Logger.h"
#include "Visual/Model/Model.h"

namespace Dindi
{	
	Mesh::Mesh(std::string vertexPath /*= DEFAULT_VERTEX_SHADER*/, std::string fragmentPath /*= DEFAULT_FRAGMENT_SHADER*/) : m_Material(nullptr)
	{
		m_Material = new Material(vertexPath, fragmentPath);
	}

	Mesh::Mesh(std::vector<vec3>&& vertices, std::string vertexPath /*= DEFAULT_VERTEX_SHADER*/, std::string fragmentPath /*= DEFAULT_FRAGMENT_SHADER*/) noexcept
	{
		m_Material = new Material(vertexPath, fragmentPath);
		m_VertexPositions = vertices;
		
		//In this case, probably we are not being created by a Model but by ourselves.
		RegisterData();
		BuildAABB();
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

	Dindi::AABB Mesh::GetOffsetAABB(const vec3& offset, const vec3& scale, const vec3& rotation)
	{
		//#TODO: Add a "Dirt" flag
		
		mat4 rotationTransform = mat4::Rotate(rotation.z, { 0.0f, 0.0f, 1.0f });
		rotationTransform     *= mat4::Rotate(rotation.y, { 0.0f, 1.0f, 0.0f });
		rotationTransform     *= mat4::Rotate(rotation.x, { 1.0f, 0.0f, 0.0f });

		mat4 transform;
		transform = mat4::Translate(offset) *  rotationTransform * mat4::Scale(scale);

		vec3 min = m_AABB.GetLocalMin();
		vec3 max = m_AABB.GetLocalMax();

		min = transform * min;
		max = transform * max;

		return { min, max};
	}

	void Mesh::SetPickablePosition(const vec3& pos)
	{
		SetPosition(m_Position + pos);
	}

	void Mesh::SetPickableRotation(const vec3& rot)
	{
		SetRotation(rot);
	}

	std::pair<Dindi::vec3, Dindi::vec3> Mesh::GetPickablePosition() const
	{
		vec3 halfAABB = (m_AABB.GetMin() + m_AABB.GetMax()) / 2;

		return { std::make_pair(halfAABB, halfAABB) };
	}

	Dindi::AABB Mesh::GetPickableAABB() const
	{
		return GetAABB();
	}


	void Mesh::BuildAABB()
	{
		vec3 minBoundaries = m_VertexPositions[0], maxBoundaries = m_VertexPositions[0];

		for (uint32_t vpi = 0; vpi < m_VertexPositions.size(); vpi++)
		{
			if (m_VertexPositions[vpi].x < minBoundaries.x)
				minBoundaries.x = m_VertexPositions[vpi].x;

			if (m_VertexPositions[vpi].y < minBoundaries.y)
				minBoundaries.y = m_VertexPositions[vpi].y;

			if (m_VertexPositions[vpi].z < minBoundaries.z)
				minBoundaries.z = m_VertexPositions[vpi].z;
		}
		
		for (uint32_t vpi = 0; vpi < m_VertexPositions.size(); vpi++)
		{
			if (m_VertexPositions[vpi].x > maxBoundaries.x)
				maxBoundaries.x = m_VertexPositions[vpi].x;

			if (m_VertexPositions[vpi].y > maxBoundaries.y)
				maxBoundaries.y = m_VertexPositions[vpi].y;

			if (m_VertexPositions[vpi].z > maxBoundaries.z)
				maxBoundaries.z = m_VertexPositions[vpi].z;
		}
		
		m_AABB = AABB(minBoundaries, maxBoundaries);

		//vec3 halfAABB = (minBoundaries + maxBoundaries) / 2;
		float xx = (minBoundaries.x + maxBoundaries.x) / 2;
		float yy = (minBoundaries.y + maxBoundaries.y) / 2;
		float zz = (minBoundaries.z + maxBoundaries.z) / 2;
		
		m_HalfWay = m_MeshOrigin = { xx, yy, zz };
	}

}