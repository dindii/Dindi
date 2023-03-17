#pragma once

class Model;
class Mesh;

namespace Dindi
{
	struct PickupContext
	{
		Model* selectedModel = nullptr;
		Mesh*  selectedMesh = nullptr;

		bool ignoreMesh = true;
	};
}
