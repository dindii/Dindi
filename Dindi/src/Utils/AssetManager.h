#pragma once
#include <string>
#include <unordered_map>
#include <memory>

namespace Dindi
{
	class AssetManager
	{
	public:

		template<class T>
		static std::shared_ptr<T> Get(const std::string& path)
		{
			if (!m_AssetsHandles[path].expired())
				return std::static_pointer_cast<T>(m_AssetsHandles[path].lock());
		
			return nullptr;
		}

		template<class T>
		static void Add(const std::string& path, std::shared_ptr<T>& asset)
		{
			m_AssetsHandles[path] = std::static_pointer_cast<void>(asset);
		}

	private:
		static std::unordered_map<std::string, std::weak_ptr<void>> m_AssetsHandles;
	};


}
