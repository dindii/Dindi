#pragma once
#include "Rendering/Object/Renderable.h"
#include "Math/vec2.h"
#include "Rendering/Texture/Texture2D.h"
#include "Visual/Material/Material.h"
#include <Physics/AABB.h>

namespace Dindi
{
	//#NOTE: Model will have many meshes support, but each mesh will have its own texture.
	
	class Mesh : public Renderable
	{
	public:
		Mesh(std::string vertexPath = DEFAULT_VERTEX_SHADER, std::string fragmentPath = DEFAULT_FRAGMENT_SHADER);
		Mesh(std::vector<vec3>&& vertices, std::string vertexPath = DEFAULT_VERTEX_SHADER, std::string fragmentPath = DEFAULT_FRAGMENT_SHADER) noexcept;
		Mesh(Material* material);
		
		~Mesh();

		inline uint32_t GetVertexCount() const { return (uint32_t)m_VertexPositions.size(); }

		std::vector<vec3>& GetVertexPositions() { return m_VertexPositions; }

		void SetVertexPositionData(std::vector<vec3>&& vertexPosData)    noexcept { m_VertexPositions = vertexPosData; BuildAABB(); }
		void SetNormalData        (std::vector<vec3>&& normalData)       noexcept { m_Normal = normalData; }
		void SetTextureCoordData  (std::vector<vec2>&& textureCoordData) noexcept { m_TextureCoord = textureCoordData; }

		inline Material* GetMaterial() { return m_Material; }

		inline void SetMaterial(Material* mat) { m_Material = mat; }

		void RegisterData();
		
		void SetAABB(const AABB& aabb) { m_AABB = aabb; }
		void SetWorldAABB(const AABB& aabb) { m_AABB.SetMin(aabb.GetMin()); m_AABB.SetMax(aabb.GetMax()); }
		AABB GetAABB() const { return m_AABB; }

		AABB GetOffsetAABB(const vec3& translation, const vec3& scale);


		inline void SetPosition(const vec3& pos) { m_Position = pos; }
		vec3 GetPosition() const { return m_Position; }


		//#NOTE: Like, this is not meant to be a game engine, but It would nice to setup some sort of residency rule to deallocate stuff from gpu memory when it needs
	private:
		void BuildAABB();

		std::vector<uint32_t> m_Indices;
		std::vector<vec3>     m_VertexPositions;
		std::vector<vec3>     m_Normal;
		std::vector<vec2>     m_TextureCoord;
		
		vec3 m_Position;

		AABB m_AABB;

		Material* m_Material;
	};
}
