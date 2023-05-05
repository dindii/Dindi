#pragma once
#include "Math/vec3.h"
#include <Interactable/Pickable.h>

namespace Dindi
{
	struct GPUPointLightData
	{
		GPUPointLightData(const vec3& pos, const vec3& color) : pLightPosition({ pos.x, pos.y, pos.z, 1.0f }), pLightColor({ color.x, color.y, color.z, 1.0f }) {};
		GPUPointLightData() = default;

		vec4 pLightPosition;
		vec4 pLightColor;
	};

	class PointLight : public Pickable
	{
	public:
		PointLight() : m_Data(nullptr) {};

		void SetPosition(const vec3& pos) { m_Data->pLightPosition = { pos.x, pos.y, pos.z, 1.0f}; }
		void SetPickablePosition(const vec3& pos) override { SetPosition(pos); }
		bool SetColor(const vec4& color);

		//#TODO overload vec4 -> vec3 constructor so this code can be cleaner
		inline vec3 GetPosition() const { return vec3(m_Data->pLightPosition.x, m_Data->pLightPosition.y, m_Data->pLightPosition.z); }
		inline vec3 GetColor()    const { return vec3(m_Data->pLightColor.x, m_Data->pLightColor.y, m_Data->pLightColor.z); }

		std::pair<vec3, vec3> GetPickablePosition() const override;
		AABB GetPickableAABB() const;

		void AssignGPULightData(GPUPointLightData* dataPtr) { m_Data = dataPtr; }
	private:
		bool AssertColor(const vec4& color);

		GPUPointLightData* m_Data;
	};
}