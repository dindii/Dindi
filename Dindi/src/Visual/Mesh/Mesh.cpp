#include "Dindipch.h"
#include "Mesh.h"
#include "Utils/Logger.h"
#include "Visual/Model/Model.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Dindi
{	
	Mesh::Mesh(std::string vertexPath /*= DEFAULT_VERTEX_SHADER*/, std::string fragmentPath /*= DEFAULT_FRAGMENT_SHADER*/) : m_Material(nullptr), m_Transform(1.0f)
	{
		m_Material = new Material(vertexPath, fragmentPath);
	}

	Mesh::Mesh(std::vector<glm::vec3>&& vertices, std::string vertexPath /*= DEFAULT_VERTEX_SHADER*/, std::string fragmentPath /*= DEFAULT_FRAGMENT_SHADER*/) noexcept : m_Transform(1.0f)
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

	Dindi::AABB Mesh::GetOffsetAABB(const glm::vec3& offset, const glm::vec3& scale, const glm::vec3& rotation)
	{
		//#TODO: Add a "Dirt" flag
		
		glm::mat4 rotationTransform(1.0f);
		rotationTransform = glm::rotate(rotationTransform, rotation.z, { 0.0f, 0.0f, 1.0f });
		rotationTransform = glm::rotate(rotationTransform, rotation.y, { 0.0f, 1.0f, 0.0f });
		rotationTransform = glm::rotate(rotationTransform, rotation.x, { 1.0f, 0.0f, 0.0f });

		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, offset) *  rotationTransform * glm::scale(transform, scale);

		glm::vec3 min = m_AABB.GetLocalMin();
		glm::vec3 max = m_AABB.GetLocalMax();

		min = transform * glm::vec4(min.x, min.y, min.z, 1.0f);
		max = transform * glm::vec4(max.x, max.y, max.z, 1.0f);

		return { min, max};
	}

	void Mesh::SetPickablePosition(const glm::vec3& pos)
	{
		SetPosition(m_Position + pos);
	}

	void Mesh::SetPickableRotation(const glm::vec3& rot)
	{
		SetRotation(rot);
	}

	std::pair<glm::vec3, glm::vec3> Mesh::GetPickablePosition() const
	{
		glm::vec3 halfAABB = (m_AABB.GetMin() + m_AABB.GetMax());

		halfAABB.x /= 2;
		halfAABB.y /= 2;
		halfAABB.z /= 2;

		return { std::make_pair(halfAABB, halfAABB) };
	}

	Dindi::AABB Mesh::GetPickableAABB() const
	{
		return GetAABB();
	}


	void Mesh::BuildAABB()
	{
		glm::vec3 minBoundaries = m_VertexPositions[0], maxBoundaries = m_VertexPositions[0];

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

		//glm::vec3 halfAABB = (minBoundaries + maxBoundaries) / 2;
		float xx = (minBoundaries.x + maxBoundaries.x) / 2.0f;
		float yy = (minBoundaries.y + maxBoundaries.y) / 2.0f;
		float zz = (minBoundaries.z + maxBoundaries.z) / 2.0f;
		
		m_HalfWay = m_MeshOrigin = { xx, yy, zz };
	}

}