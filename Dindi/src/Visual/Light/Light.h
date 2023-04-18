#pragma once
#include "Math/vec3.h"
#include <Interactable/Pickable.h>

namespace Dindi
{
	class PointLight : public Pickable
	{
	public:
		PointLight(const vec4& position, const vec4& color);
		PointLight(const vec3& position, const vec3& color);
		PointLight() = default;

		void SetPosition(const vec3& pos) { m_Position = { pos.x, pos.y, pos.z, 1.0f}; }
		void SetPickablePosition(const vec3& pos) override { SetPosition(pos); }
		bool SetColor(const vec4& color);
		//bool SetStrength(const float strength);

		inline vec3 GetPosition() const { return vec3(m_Position.x, m_Position.y, m_Position.z); }
		inline vec3 GetColor() const { return vec3(m_Color.x, m_Color.y, m_Color.z); }

		inline vec4& GetPositionRef() { return m_Position; }

		std::pair<vec3, vec3> GetPickablePosition() const override;
		AABB GetPickableAABB() const;

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