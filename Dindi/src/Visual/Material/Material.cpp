#include "Dindipch.h"
#include "Material.h"


namespace Dindi
{

	Material::Material()
	{
		//m_diffuseMap   = new Texture2D();
		//m_normalMap    = new Texture2D();
		//m_roughnessMap =	new Texture2D();
		//m_specularMap  =		new Texture2D();
		//m_ambientOcclusionMap = new Texture2D();;
	}

	void Material::Bind() const
	{
		m_Shader->Bind();

		if (m_diffuseMap)
		{
			m_diffuseMap->Bind(				 RenderingMapSlot::Diffuse);
			m_Shader->UploadInt("u_Diffuse", RenderingMapSlot::Diffuse);
		}

		if (m_specularMap)
		{
			m_specularMap->Bind(	          RenderingMapSlot::Specular);
			m_Shader->UploadInt("u_Specular", RenderingMapSlot::Specular);
		}

		if (m_normalMap)
		{
			m_normalMap->Bind(				RenderingMapSlot::Normal);
			m_Shader->UploadInt("u_Normal", RenderingMapSlot::Normal);
		}

		//#TODO: add more.
		//#NOTE: Besides the obrigatory stuff like Time and ViewProjection that we
		//upload to the gpu right away,
		//the user will send different parameters to this material
		//and they all will be uploaded here (from member class data to glsl)
	}

	void Material::SetDiffuseMap(Texture2D* DiffuseMap)
	{
		m_diffuseMap = DiffuseMap;
	}

	void Material::SetNormalMap(Texture2D* NormalMap)
	{
		m_normalMap = NormalMap;
	}

	void Material::SetRoughnessMap(Texture2D* RoughnessMap)
	{
		m_roughnessMap = RoughnessMap;
	}

	void Material::SetSpecularMap(Texture2D* SpecularMap)
	{
		m_specularMap = SpecularMap;
	}

	void Material::SetAmbientOcclusionMap(Texture2D* AmbientOcclusionMap)
	{
		m_ambientOcclusionMap = AmbientOcclusionMap;
	}

	void Material::SetViewProjection(const mat4& mat)
	{
		m_Shader->UploadUniformMat4("u_ViewProjection", mat);
	}

	void Material::SetTime(float time)
	{
		m_Shader->UploadUniformFloat("u_Time", time);
	}

}