#include "Dindipch.h" 
#include "ShaderHotReloader.h" 

//#NOTE This probably allocate std::string memory, check this when we have a bottleneck. 
//#NOTE This runs in a single thread, the same as the renderer. Please, put this in another thread and insert a sleep for it. 

//#TODO Combine shaders in a single file to add vertex shader hot reloader as well. I am too lazy to do this right now because I would have to 
//change some shader loading stuff. 
//I will probably do this transition once I miss vertex shader hot reload ha. 
#define LAST_SHADER_WRITE_TIME(x)  std::filesystem::last_write_time(std::filesystem::path(x.GetFragmentShaderFilePath())).time_since_epoch().count() 

namespace Dindi
{

#if DINDI_DEBUG
	std::vector<int64_t> ShaderHotReloader::m_LastTimeEdited;
	std::vector<Shader*> ShaderHotReloader::m_DirtyShaders;
	std::vector<Shader*> ShaderHotReloader::m_Shaders;
#endif

	void ShaderHotReloader::Init()
	{
	}

	void ShaderHotReloader::OnUpdate()
	{
		CheckForShaderFileChanges();
		ReloadDirtyShaders();
	}

	void ShaderHotReloader::AddShader(Shader& shader)
	{
		m_Shaders.emplace_back(&shader);
		m_LastTimeEdited.emplace_back(LAST_SHADER_WRITE_TIME(shader));
	}

	void ShaderHotReloader::RemoveShader(Shader& shader)
	{
		for (uint32_t i = 0; i < m_Shaders.size(); i++)
		{
			if (m_Shaders[i]->GetID() == shader.GetID())
			{
				m_Shaders.erase(m_Shaders.begin() + i);
				m_LastTimeEdited.erase(m_LastTimeEdited.begin() + i);
				break;
			}
		}
	}

	void ShaderHotReloader::CheckForShaderFileChanges()
	{
		for (uint32_t x = 0; x < m_Shaders.size(); x++)
		{
			auto OS_lastWriteTime = LAST_SHADER_WRITE_TIME((*m_Shaders[x]));
			auto lastWriteTimeAccounted = m_LastTimeEdited[x];

			if (lastWriteTimeAccounted != 0 && lastWriteTimeAccounted < OS_lastWriteTime)
			{
				m_DirtyShaders.emplace_back(m_Shaders[x]);
				m_LastTimeEdited[x] = OS_lastWriteTime;
			}
		}
	}

	void ShaderHotReloader::ReloadDirtyShaders()
	{
		//This is kinda linear but ok, I don't think it will be a problem. 
		for (uint32_t x = 0; x < m_DirtyShaders.size(); x++)
		{
			m_DirtyShaders[x]->ReloadShader();
			m_DirtyShaders.erase(m_DirtyShaders.begin() + x);
		}
	}
}