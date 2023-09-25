#pragma once

#include <glm/vec3.hpp>
#include <utility>
#include <Physics/AABB.h>

namespace Dindi
{
	class Pickable
	{
	public:
		virtual std::pair<glm::vec3, glm::vec3> GetPickablePosition() const = 0;
		virtual glm::vec3 GetPickableRotation() const = 0;
		virtual float GetPickableScale() const = 0;
		virtual AABB GetPickableAABB() const = 0;

		virtual void SetPickablePosition(const glm::vec3& newPos) = 0;
		virtual void SetPickableRotation(const glm::vec3& rot) = 0;
		virtual void SetPickableScale(float scale) = 0;

		virtual bool GetPickableDirty() const { return m_Dirty; }
		virtual void SetPickableDirty(bool dirty) { m_Dirty = dirty; }

	protected:
		bool m_Dirty = true;
	};

}
