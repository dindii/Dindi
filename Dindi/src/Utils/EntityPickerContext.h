#pragma once
#include <stdint.h>

class Model;
class Mesh;

namespace Dindi
{
	enum class EntityPickerMode  : uint8_t
	{
		MeshOnly = 0,
		ModelOnly

	};
	struct EntityPickerContext
	{
		Model* selectedModel = nullptr;
		Mesh*  selectedMesh = nullptr;

		bool ignoreMesh = true;

	private:
		uint8_t _pad[7];
	};
}

