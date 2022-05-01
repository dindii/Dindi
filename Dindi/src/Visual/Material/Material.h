#pragma once
#include "Rendering/Shader/Shader.h"
#include <string>
namespace Dindi
{
	class Material
	{
	public:
		//The idea here is an interface for Shader, to easily tweak some attributes and exclude de necessity of 
		//having shaders all over the place
		Material(std::string vertexPath, std::string fragmentPath) { m_Shader = new Shader(vertexPath, fragmentPath); };
		Material(Shader* shader) : m_Shader(shader) {};

		inline Shader* GetShader() { return m_Shader; }

	private:
		Shader* m_Shader;
	};
}
