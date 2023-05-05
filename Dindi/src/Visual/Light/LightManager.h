#pragma once

#include "Light.h"
#include <vector>
#include "Math/vec3.h"

namespace Dindi
{
	class LightManager
	{
	public:
		LightManager();

		void AddPointLight(const vec3& position, const vec3& color);
		void RemoveLight(uint32_t idx);

		std::vector<PointLight>& GetLights() { return m_Lights; }
		std::vector<GPUPointLightData>& GetLightsData() { return m_LightsData; }
	
	private:
		std::vector<PointLight> m_Lights;
		std::vector<GPUPointLightData> m_LightsData;

		bool m_Initialized = false;
	};
}