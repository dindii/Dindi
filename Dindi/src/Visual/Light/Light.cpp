#include "Dindipch.h"
#include "Light.h"
#include "Core/Core.h"
#include "Utils/Logger.h"

namespace Dindi
{

	//PointLight::PointLight(const vec3& position, const vec3& color, const float strength) : m_Position(position)
	//{
	//	SetColor(color);
	//	SetStrength(strength);
	//}

	PointLight::PointLight(const vec4& position, const vec4& color) : m_Position(position)
	{
		SetColor(color);
	}

	PointLight::PointLight(const vec3& position, const vec3& color) : m_Position(position.x, position.y, position.z, 0.0f)
	{
		SetColor({ color.x, color.y, color.z, 0.0f });
	}

	bool PointLight::SetColor(const vec4& color)
	{
		if (!AssertColor(color))
		{
			m_Color = color;
			return true;
		}

		DND_LOG_WARNING("There's a pitch black or negative light, is this your intent? Skipping this one.");
		return false;
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