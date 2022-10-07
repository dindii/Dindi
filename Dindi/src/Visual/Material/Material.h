#pragma once
#include "Rendering/Shader/Shader.h"
#include <string>
#include "Rendering/Texture/Texture2D.h"
#include <Core/Core.h>

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
		inline Ref<Shader>& GetShader()  { return m_Shader; }

		void SetDiffuseMap         (Ref<Texture2D> DiffuseMap);
		void SetNormalMap          (Ref<Texture2D> NormalMap);
		void SetRoughnessMap       (Ref<Texture2D> RoughnessMap);
		void SetSpecularMap        (Ref<Texture2D> SpecularMap);
		void SetAmbientOcclusionMap(Ref<Texture2D> AmbientOcclusionMap);

	private:
		Ref<Shader> m_Shader;

		Ref<Texture2D> m_diffuseMap;
		Ref<Texture2D> m_normalMap;
		Ref<Texture2D> m_roughnessMap;
		Ref<Texture2D> m_specularMap;
		Ref<Texture2D> m_ambientOcclusionMap;
	};
}
