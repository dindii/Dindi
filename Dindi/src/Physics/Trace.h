#pragma once

#include <Rendering/Camera/Camera.h>

namespace Dindi
{
	class Trace
	{
	public:
		Trace(const float x, const float y, const Camera& camera);
		~Trace() = default;

		vec3 GetDirection() const { return m_Coords; }


		static vec3 CastRay(const float x, const float y, const Camera& camera);


	private:
		vec3 m_Coords;
	};
}