#pragma once
#include "Math/vec3.h"

namespace Dindi
{
	class PointLight
	{
	public:
		PointLight(const vec3& position, const vec3& color, const float strength);

		void SetPosition(const vec3& pos) { m_Position = pos; }
		bool SetColor(const vec3& color);
		bool SetStrength(const float strength);

		inline vec3 GetPosition() const { return m_Position; }
		inline vec3 GetColor() const { return m_Color; }

	private:
		bool AssertColor(const vec3& color);
		bool AssertStrength(const float strength);
	private:
		vec3 m_Position, m_Color;
		float m_Strength;

		float padding;
		//Add a radius, maybe?
	};
}