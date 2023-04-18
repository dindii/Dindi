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
		virtual AABB GetPickableAABB() const = 0;
		virtual void SetPickablePosition(const vec3& newPos) = 0;
	};

}
