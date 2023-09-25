#pragma once
#include "glm/vec3.hpp"
#include <Interactable/Pickable.h>

namespace Dindi
{
	struct GPUPointLightData
	{
		GPUPointLightData(const glm::vec3& pos, const glm::vec3& color) : pLightPosition({ pos.x, pos.y, pos.z, 1.0f }), pLightColor({ color.x, color.y, color.z, 1.0f }) {};
		GPUPointLightData() = default;

		glm::vec4 pLightPosition;
		glm::vec4 pLightColor;
	};

	class PointLight : public Pickable
	{
	public:
		PointLight() : m_Data(nullptr) {};

		void SetPosition(const glm::vec3& pos) { m_Data->pLightPosition = { pos.x, pos.y, pos.z, 1.0f}; }
		void SetPickablePosition(const glm::vec3& pos) override { SetPosition(pos); }
		void SetPickableRotation(const glm::vec3& pos) override {};
		inline virtual void SetPickableScale(const float scale) override {};

		bool SetColor(const glm::vec4& color);

		//#TODO overload vec4 -> glm::vec3 constructor so this code can be cleaner
		inline glm::vec3 GetPosition() const { return glm::vec3(m_Data->pLightPosition.x, m_Data->pLightPosition.y, m_Data->pLightPosition.z); }
		inline glm::vec3 GetColor()    const { return glm::vec3(m_Data->pLightColor.x, m_Data->pLightColor.y, m_Data->pLightColor.z); }

		virtual std::pair<glm::vec3, glm::vec3> GetPickablePosition() const override;
		virtual glm::vec3 GetPickableRotation() const override { return {}; }
		inline virtual float GetPickableScale() const override { return 1.0f; }

		AABB GetPickableAABB() const;

		void AssignGPULightData(GPUPointLightData* dataPtr) { m_Data = dataPtr; }
	private:
		bool AssertColor(const glm::vec4& color);

		GPUPointLightData* m_Data;
	};
}