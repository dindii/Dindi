#include "Dindipch.h"
#include "LightManager.h"
#include <Core/Core.h>

namespace Dindi
{
	LightManager::LightManager()
	{
		if (!m_Initialized)
		{
			m_Initialized = true;

			m_LightsData.reserve(DND_MAX_LIGHTS);
			m_Lights.reserve(DND_MAX_LIGHTS);
		}
	}

	void LightManager::AddPointLight(const vec3& position, const vec3& color)
	{
		m_Lights.push_back({});
		uint32_t lastLightIdx = m_Lights.size() - 1;

		m_LightsData.push_back({ position, color });
		uint32_t lastDataIdx = m_LightsData.size() - 1;

		m_Lights[lastLightIdx].AssignGPULightData(&m_LightsData[lastDataIdx]);
	}

	void LightManager::RemoveLight(uint32_t idx)
	{
		m_LightsData.erase(m_LightsData.begin() + idx);
		
		if(idx + 1 < m_Lights.size())
			m_Lights[idx + 1].AssignGPULightData(&m_LightsData[idx]);

		m_Lights.erase(m_Lights.begin() + idx);
	}

}