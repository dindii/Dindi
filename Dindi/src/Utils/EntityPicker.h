#pragma once
#include "EntityPickerContext.h"


namespace Dindi
{
	class EntityPicker
	{
	public:
		EntityPicker() = delete;
		~EntityPicker() = delete;

		static const EntityPickerContext PickEntity(const EntityPickerMode mode);
		static const EntityPickerContext GetLatestPickedEntity() { return m_CachedEntity; }

		static inline void SetPickerRayMaxSteps(uint32_t steps) { m_RayMaxSteps = steps; }
		static inline void SetAdditionalRayDistancePerStep(uint32_t additionalRayDistance) { m_AdditionalDistancePerStep = additionalRayDistance; }
	private: 
		static EntityPickerContext m_CachedEntity;

		static uint32_t m_RayMaxSteps;
		static float m_AdditionalDistancePerStep;
	};

}