#include "Dindipch.h"
#include "Light.h"
#include "Core/Core.h"
#include "Utils/Logger.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Dindi
{
	bool PointLight::SetColor(const glm::vec4& color)
	{
		if (!AssertColor(color))
		{
			m_Data->pLightColor = color;
			return true;
		}

		DND_LOG_WARNING("There's a pitch black or negative light, is this your intent? Skipping this one.");
		return false;
	}

	std::pair<glm::vec3, glm::vec3> PointLight::GetPickablePosition() const
	{
		glm::vec4 pos4 = m_Data->pLightPosition;

		glm::vec3 pos = { pos4.x, pos4.y, pos4.z };

		return { std::make_pair(pos, glm::vec3(0.0f)) };
	}
	

	Dindi::AABB PointLight::GetPickableAABB() const
	{
		glm::vec3 lightMin(-1.0f, -1.0f, -1.0f);
		glm::vec3 lightMax(1.0f, 1.0f, 1.0f);

		glm::vec4 position4 = m_Data->pLightPosition;
		glm::vec3 position = { position4.x, position4.y, position4.z };

		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, position);

		lightMin = transform * glm::vec4(lightMin.x, lightMin.y, lightMin.z, 1.0f);
		lightMax = transform * glm::vec4(lightMax.x, lightMax.y, lightMax.z, 1.0f);

		return { lightMin, lightMax };
	}

	//bool PointLight::SetStrength(const float strength)
	//{
	//	if (AssertStrength(strength))
	//	{
	//		m_Strength = strength;
	//		return true;
	//	}
	//
	//	DND_LOG_WARNING("There's a light with zero or negative strength, is this your intent? Skipping this one.");
	//	return false;
	//}

	bool PointLight::AssertColor(const glm::vec4& color)
	{
		return (color.r <= 0.0f && color.g <= 0.0f && color.b <= 0.0f);
	}

	//bool PointLight::AssertStrength(const float strength)
	//{
	//	return (strength > 0.0f);
	//}

}