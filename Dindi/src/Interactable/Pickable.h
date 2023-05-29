#pragma once

#include <Math/vec3.h>
#include <utility>
#include <Physics/AABB.h>

namespace Dindi
{
	class Pickable
	{
	public:
		virtual std::pair<vec3, vec3> GetPickablePosition() const = 0;
		virtual vec3 GetPickableRotation() const = 0;
		virtual float GetPickableScale() const = 0;
		virtual AABB GetPickableAABB() const = 0;

		virtual void SetPickablePosition(const vec3& newPos) = 0;
		virtual void SetPickableRotation(const vec3& rot) = 0;
		virtual void SetPickableScale(float scale) = 0;

		virtual bool GetPickableDirty() const { return m_Dirty; }
		virtual void SetPickableDirty(bool dirty) { m_Dirty = dirty; }

	protected:
		bool m_Dirty = true;
	};

}
