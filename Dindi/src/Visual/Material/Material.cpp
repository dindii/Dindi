#include "Dindipch.h"
#include <Rendering/Core/Common.hpp>
#include "Material.h"

#include <Rendering/Core/Renderer.h>
#include <Rendering/RenderPasses/Shadow/CascadedShadowMapRenderPass.h>

namespace Dindi
{


	Material::Material(const std::string& vertexpath, const std::string& fragpath)
	{
		m_Shader = Shader::Load(vertexpath, fragpath);
	}

	Material::~Material()
	{
		//if (m_diffuseMap)
		//	delete m_diffuseMap;
		//
		//if (m_specularMap)
		//	delete m_specularMap;
		//
		//if (m_normalMap)
		//	delete m_normalMap;
		//
		//if (m_roughnessMap)
		//	delete m_roughnessMap;
		//
		//if (m_ambientOcclusionMap)
		//	delete m_ambientOcclusionMap;
	}

	void Material::Bind() const
	{
		m_Shader->Bind();

		if (m_diffuseMap)
		{
			m_diffuseMap->Bind(				 ERenderingMapSlot::Diffuse);
			m_Shader->UploadInt("u_Diffuse", ERenderingMapSlot::Diffuse);
		}

		if (m_specularMap)
		{
			m_specularMap->Bind(	          ERenderingMapSlot::Specular);
			m_Shader->UploadInt("u_Specular", ERenderingMapSlot::Specular);
		}

		if (m_normalMap)
		{
			m_normalMap->Bind(				ERenderingMapSlot::Normal);
			m_Shader->UploadInt("u_Normal", ERenderingMapSlot::Normal);
		}

		

		//Texture2D& shadowMap = Renderer::GetShadowMap();
		//if (shadowMap.GetID())
		//{
		//	shadowMap.Bind(                    ERenderingMapSlot::Shadow);
		//	m_Shader->UploadInt("u_ShadowMap", ERenderingMapSlot::Shadow);
		//}


		std::vector<Texture2D*>& shadowMaps = Renderer::GetShadowMap();
		
		if (shadowMaps.size() > 0)
		{
			for (uint32_t i = 0; i < shadowMaps.size(); i++)
			{
				glBindTextureUnit(ERenderingMapSlot::Shadow + i, shadowMaps[i]->GetID());
				char shadowmapIndex[128];
				sprintf(shadowmapIndex, "u_ShadowMap[%i]", i);
				m_Shader->UploadInt(shadowmapIndex, ERenderingMapSlot::Shadow + i);
			}
		}

		uint32_t randomTextureHandle = CSMRenderPass::GetRandomAnglesTexture();
		glBindTextureUnit(ERenderingMapSlot::RandomAngles, randomTextureHandle);
		m_Shader->UploadInt("u_RandomAngles", ERenderingMapSlot::RandomAngles);


		//#TODO: add more.
		//#NOTE: Besides the obrigatory stuff like Time and ViewProjection that we
		//upload to the gpu right away,
		//the user will send different parameters to this material
		//and they all will be uploaded here (from member class data to glsl)
	}

	void Material::SetDiffuseMap(Ref<Texture2D> DiffuseMap)
	{
		m_diffuseMap = DiffuseMap;
	}

	void Material::SetNormalMap(Ref<Texture2D> NormalMap)
	{
		m_normalMap = NormalMap;
	}

	void Material::SetRoughnessMap(Ref<Texture2D> RoughnessMap)
	{
		m_roughnessMap = RoughnessMap;
	}

	void Material::SetSpecularMap(Ref<Texture2D> SpecularMap)
	{
		m_specularMap = SpecularMap;
	}

	void Material::SetAmbientOcclusionMap(Ref<Texture2D> AmbientOcclusionMap)
	{
		m_ambientOcclusionMap = AmbientOcclusionMap;
	}
}
