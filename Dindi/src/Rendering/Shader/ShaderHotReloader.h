#pragma once
#include "Rendering/Shader/Shader.h"
#include <filesystem>
#include <vector>

namespace Dindi
{
	class ShaderHotReloader
	{
	public:
		ShaderHotReloader() = default;
	
		static void OnUpdate();
		static void AddShader(Shader& shader);
	
	private:
		static void CheckForShaderFileChanges();
		static void ReloadDirtyShaders();
		
		static std::vector<int64_t> m_LastTimeEdited;
		static std::vector<Shader*> m_DirtyShaders;
		static std::vector<Shader*> m_Shaders;
	};
}