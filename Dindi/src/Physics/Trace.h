#pragma once

#include <Rendering/Camera/Camera.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Dindi
{
	class Trace
	{
	public:
		Trace(const float x, const float y, const Camera& camera);
		~Trace() = default;

		glm::vec3 GetDirection() const { return m_Coords; }


		static glm::vec3 CastRay(const float x, const float y, const Camera& camera);
		static glm::vec3 CastRay(const float x, const float y, const float width, const float height, const glm::mat4& projection, const glm::mat4& view);


	private:
		glm::vec3 m_Coords;
	};
}