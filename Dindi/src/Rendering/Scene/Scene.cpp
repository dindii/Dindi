#include "Dindipch.h"
#include "Scene.h"

namespace Dindi
{

	Scene::Scene()
	{
		//#NOTE: For now I will just upload the index with the array of lights with this same max. 
		//Maybe in the future I will consider the usage of storage buffer and stuff like that, but I need first to analyze the cost of it.
		m_Lights.reserve(DND_MAX_LIGHTS);
	}

}