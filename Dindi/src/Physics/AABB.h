#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <Utils/Logger.h>

namespace Dindi
{
	class AABB
	{
	public:
		AABB(const glm::vec3& min, const glm::vec3& max) : m_Min(min), m_Max(max), m_LocalMin(min), m_LocalMax(max) {};
		AABB() : m_LocalMin({ 0.0f, 0.0f, 0.0f }), m_LocalMax({ 0.0f, 0.0f, 0.0f }), m_Min({ 0.0f, 0.0f, 0.0f }), m_Max({ 0.0f, 0.0f, 0.0f }) {};

		bool CheckCollision(const glm::vec3& point);

		glm::vec3 GetMin() const { return m_Min; }
		glm::vec3 GetMax() const { return m_Max; }

		void SetMin(const glm::vec3& min) { m_Min = min; }
		void SetMax(const glm::vec3& max) { m_Max = max; }

		glm::vec3 GetLocalMin() const { return m_LocalMin; }
		glm::vec3 GetLocalMax() const { return m_LocalMax; }

		void SetLocalMin(const glm::vec3& localMin) { m_LocalMin = localMin; }
		void SetLocalMax(const glm::vec3& localMax) { m_LocalMax = localMax; }

		AABB& operator*=(const glm::mat4& other) { return Multiply(other); }

		float GetAABBSize() const;
	
	private:
		AABB& Multiply(const glm::mat4& mat);

	private:
		glm::vec3 m_LocalMin, m_LocalMax;
		glm::vec3 m_Min, m_Max;
	};
}