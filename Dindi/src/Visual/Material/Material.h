#pragma once
#include "Rendering/Shader/Shader.h"
#include <string>
#include "Rendering/Texture/Texture2D.h"

namespace Dindi
{
	class Material
	{
	public:
		//The idea here is an interface for Shader, to easily tweak some attributes and exclude de necessity of 
		//having shaders all over the place
		Material(const std::string& vertexpath, const std::string& fragpath);
		Material(Shader* shader) : m_Shader(shader) {};
		~Material();
		void Bind() const;
		inline Shader* GetShader() const { return m_Shader; }

		void SetDiffuseMap         (Texture2D* DiffuseMap);
		void SetNormalMap          (Texture2D* NormalMap);
		void SetRoughnessMap       (Texture2D* RoughnessMap);
		void SetSpecularMap        (Texture2D* SpecularMap);
		void SetAmbientOcclusionMap(Texture2D* AmbientOcclusionMap);

	private:
		Shader* m_Shader;

		Texture2D* m_diffuseMap		     = nullptr;
		Texture2D* m_normalMap		     = nullptr;
		Texture2D* m_roughnessMap	     = nullptr;
		Texture2D* m_specularMap	     = nullptr;
		Texture2D* m_ambientOcclusionMap = nullptr;

	};
}
