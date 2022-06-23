#pragma once
#include "Math/vec3.h"

namespace Dindi
{
	class PointLight
	{
	public:
		PointLight(const vec4& position, const vec4& color);
		PointLight() = default;

		void SetPosition(const vec4& pos) { m_Position = pos; }
		bool SetColor(const vec4& color);
		//bool SetStrength(const float strength);

		inline vec3 GetPosition() const { return m_Position; }
		inline vec3 GetColor() const { return m_Color; }

	private:
		bool AssertColor(const vec4& color);
	//	bool AssertStrength(const float strength);
	private:
		//vec3 m_Position, m_Color;
		//I'm removing the pad and using they as a vec4 because of the std140 rules so we can upload this data all at once to the gpu without too much bureaucracy.
		vec4 m_Position, m_Color;
		//float m_Strength;
	};
}