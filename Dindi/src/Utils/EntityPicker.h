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
	private: 
		static EntityPickerContext m_CachedEntity;
	};

}