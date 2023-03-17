#include "Dindipch.h"
#include "AABB.h"
#include <Math/vec4.h>
namespace Dindi
{
	bool AABB::CheckCollision(const vec3& point)
	{
		return (
			point.x >= m_Min.x && point.x <= m_Max.x &&
			point.y >= m_Min.y && point.y <= m_Max.y &&
			point.z >= m_Min.z && point.z <= m_Max.z
			);
	}

	float AABB::GetAABBSize() const
	{
		return m_Min.Distance(m_Max);
	}

	Dindi::AABB& AABB::Multiply(const mat4& mat)
	{
		m_Min = mat * m_Min;
		m_Max = mat * m_Max;

		return *this;
	}

}