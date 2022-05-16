#include "Dindipch.h"
#include "Light.h"
#include "Core/Core.h"
#include "Utils/Logger.h"

namespace Dindi
{

	PointLight::PointLight(const vec3& position, const vec3& color, const float strength) : m_Position(position)
	{
		SetColor(color);
		SetStrength(strength);
	}

	bool PointLight::SetColor(const vec3& color)
	{
		if (AssertColor(color))
		{
			m_Color = color;
			return true;
		}

		DND_LOG_WARNING("There's a pitch black or negative light, is this your intent? Skipping this one.");
		return false;
	}

	bool PointLight::SetStrength(const float strength)
	{
		if (AssertStrength(strength))
		{
			m_Strength = strength;
			return true;
		}

		DND_LOG_WARNING("There's a light with zero or negative strength, is this your intent? Skipping this one.");
		return false;
	}

	bool PointLight::AssertColor(const vec3& color)
	{
		return (color.r > 0.0f && color.g > 0.0f && color.b > 0.0f);
	}

	bool PointLight::AssertStrength(const float strength)
	{
		return (strength > 0.0f);
	}

}