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
		Mesh() = default;
		Mesh(std::string& filepath);
		inline uint32_t GetVertexCount() const { return (uint32_t)m_VertexPositions.size(); }

		void SetVertexPositionData(std::vector<vec3>&& vertexPosData)    { m_VertexPositions = vertexPosData; }
		void SetNormalData        (std::vector<vec3>&& normalData)       { m_Normal = normalData; }
		void SetTextureCoordData  (std::vector<vec2>&& textureCoordData) { m_TextureCoord = textureCoordData; }

		void RegisterData();
	private:
		std::vector<uint32_t> m_Indices;
		std::vector<vec3>     m_VertexPositions;
		std::vector<vec3>     m_Normal;
		std::vector<vec2>     m_TextureCoord;
	};
}
