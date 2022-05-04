#pragma once
#include "Rendering/Object/Renderable.h"
#include "Math/vec2.h"
#include "Rendering/Texture/Texture2D.h"

namespace Dindi
{
	//#NOTE: Model will have many meshes support, but each mesh will have its own texture.
	
	class Mesh : public Renderable
	{
	public:
		//We will have something here to init
		//We'll extent this or add a copy constructor soon
		Mesh(const std::vector<vec3>& vertexPositions);
		Mesh(std::string& filepath);
		void BindTextures() const;
		//#TODO REMOVE
		uint32_t GetVertexCount() { return (uint32_t)m_VertexPositions.size(); }

		bool HasNormal() const { return m_bHasNormal; }
		bool HasTexCoord() const { return m_bHasTexCoord; }
		std::vector<uint32_t>& GetIndices() { return m_Indices; }
		//this will be dynamic as well
	private:
		std::vector<uint32_t> m_Indices;
		std::vector<vec3> m_VertexPositions;
		std::vector<vec3> m_Normal;
		std::vector<vec2> m_TextureCoord;

		Texture2D* m_diffuseMap          = nullptr;
		Texture2D* m_normalMap           = nullptr;
		Texture2D* m_roughnessMap        = nullptr;
		Texture2D* m_specularMap         = nullptr;
		Texture2D* m_ambientOcclusionMap = nullptr;


		bool m_bHasNormal   = false;
		bool m_bHasTexCoord = false;
	};
}
