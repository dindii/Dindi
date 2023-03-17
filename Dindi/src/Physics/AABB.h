#pragma once

#include <Math/vec3.h>
#include <Math/mat4.h>
#include <Utils/Logger.h>

namespace Dindi
{
	class AABB
	{
	public:
		AABB(const vec3& min, const vec3& max) : m_Min(min), m_Max(max), m_LocalMin(min), m_LocalMax(max) {};
		AABB() = default;

		bool CheckCollision(const vec3& point);

		vec3 GetMin() const { return m_Min; }
		vec3 GetMax() const { return m_Max; }

		void SetMin(const vec3& min) { m_Min = min; }
		void SetMax(const vec3& max) { m_Max = max; }

		vec3 GetLocalMin() const { return m_LocalMin; }
		vec3 GetLocalMax() const { return m_LocalMax; }

		void SetLocalMin(const vec3& localMin) { m_LocalMin = localMin; }
		void SetLocalMax(const vec3& localMax) { m_LocalMax = localMax; }

		AABB& operator*=(const mat4& other) { return Multiply(other); }

		float GetAABBSize() const;
	
	private:
		AABB& Multiply(const mat4& mat);

	private:
		vec3 m_LocalMin, m_LocalMax;
		vec3 m_Min, m_Max;
	};
}