#pragma once
#include <stdint.h>

class Pickable;

namespace Dindi
{
	enum class EntityPickerMode  : uint8_t
	{
		MeshOnly = 0,
		ModelOnly

	};

	struct EntityPickerContext
	{
		Pickable* pickedEntity = nullptr;
	};
}

