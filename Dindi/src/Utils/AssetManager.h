#pragma once
#include <string>
#include <unordered_map>
namespace Dindi
{
	class AssetManager
	{
	public:

		template<class T>
		static T* Get(const std::string& path)
		{
			if (m_AssetsHandles[path])
				return static_cast<T*>(m_AssetsHandles[path]);
		
			return nullptr;
		}

		template<class T>
		static void Add(const std::string& path, T* asset)
		{
			m_AssetsHandles[path] = static_cast<void*>(asset);
		}

	private:
		static std::unordered_map<std::string, void*> m_AssetsHandles;
	};


}
