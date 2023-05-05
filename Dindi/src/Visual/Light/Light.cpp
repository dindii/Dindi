#include "Dindipch.h"
#include "Light.h"
#include "Core/Core.h"
#include "Utils/Logger.h"

namespace Dindi
{
	bool PointLight::SetColor(const vec4& color)
	{
		if (!AssertColor(color))
		{
			m_Data->pLightColor = color;
			return true;
		}

		DND_LOG_WARNING("There's a pitch black or negative light, is this your intent? Skipping this one.");
		return false;
	}

	std::pair<Dindi::vec3, Dindi::vec3> PointLight::GetPickablePosition() const
	{
		vec4 pos4 = m_Data->pLightPosition;

		vec3 pos = { pos4.x, pos4.y, pos4.z };

		return { std::make_pair(pos, vec3(0.0f)) };
	}
	

	Dindi::AABB PointLight::GetPickableAABB() const
	{
		vec3 lightMin(-1.0f, -1.0f, -1.0f);
		vec3 lightMax(1.0f, 1.0f, 1.0f);

		vec4 position4 = m_Data->pLightPosition;
		vec3 position = { position4.x, position4.y, position4.z };

		mat4 transform = mat4::Translate(position);

		lightMin = transform * lightMin;
		lightMax = transform * lightMax;

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

	bool PointLight::AssertColor(const vec4& color)
	{
		return (color.r <= 0.0f && color.g <= 0.0f && color.b <= 0.0f);
	}

	//bool PointLight::AssertStrength(const float strength)
	//{
	//	return (strength > 0.0f);
	//}

}