#include "Dindipch.h"
#include "AABB.h"
#include <Math/Maths.h>

namespace Dindi
{
	bool AABB::CheckCollision(const glm::vec3& point)
	{
		return (
			point.x >= m_Min.x && point.x <= m_Max.x &&
			point.y >= m_Min.y && point.y <= m_Max.y &&
			point.z >= m_Min.z && point.z <= m_Max.z
			);
	}

	float AABB::GetAABBSize() const
	{
		return Distance(m_Min, m_Max);
	}

	Dindi::AABB& AABB::Multiply(const glm::mat4& mat)
	{
		m_Min = mat * glm::vec4(m_Min.x, m_Min.y, m_Min.z, 1.0f);
		m_Max = mat * glm::vec4(m_Max.x, m_Max.y, m_Max.z, 1.0f);

		return *this;
	}

}