#include "Dindipch.h"
#include "AssetManager.h"

namespace Dindi
{
	std::unordered_map<std::string, std::weak_ptr<void>> AssetManager::m_AssetsHandles;
}