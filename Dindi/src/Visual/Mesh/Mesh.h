#pragma once
#include "Rendering/Object/Renderable.h"
#include "Rendering/Texture/Texture2D.h"
#include "Visual/Material/Material.h"
#include <Physics/AABB.h>
#include <Interactable/Pickable.h>


namespace Dindi
{
	//#NOTE: Model will have many meshes support, but each mesh will have its own texture.
	
	class Mesh : public Renderable, public Pickable
	{
	public:
		Mesh();
		Mesh(std::string vertexPath, std::string fragmentPath);
		Mesh(std::vector<glm::vec3>&& vertices, std::string vertexPath, std::string fragmentPath) noexcept;
		
		~Mesh();

		inline uint32_t GetVertexCount() const { return (uint32_t)m_VertexPositions.size(); }

		std::vector<glm::vec3>& GetVertexPositions() { return m_VertexPositions; }

		void SetVertexPositionData(std::vector<glm::vec3>&& vertexPosData)    noexcept { m_VertexPositions = vertexPosData; BuildAABB(); m_Dirty = true; }
		void SetNormalData        (std::vector<glm::vec3>&& normalData)       noexcept { m_Normal = normalData; m_Dirty = true;}
		void SetTextureCoordData  (std::vector<glm::vec2>&& textureCoordData) noexcept { m_TextureCoord = textureCoordData; m_Dirty = true;}
		void SetTangentData(std::vector<glm::vec3>&& tangentData)             noexcept { m_Tangent = tangentData; m_Dirty = true;}

		inline Material* GetMaterial() { return m_Material; }

		inline void SetMaterial(Material* mat) { m_Material = mat; }

		void RegisterData();
		
		bool GetIsDirty() { return m_Dirty; }

		void SetAABB(const AABB& aabb) { m_AABB = aabb; }
		void SetWorldAABB(const AABB& aabb) { m_AABB.SetMin(aabb.GetMin()); m_AABB.SetMax(aabb.GetMax()); }
		AABB GetAABB() const { return m_AABB; }

		AABB GetOffsetAABB(const glm::vec3& translation, const glm::vec3& scale, const glm::vec3& rotation);


		inline void SetPosition(const glm::vec3& pos) { m_Position = pos; }
		inline void SetRotation(const glm::vec3& rot) { m_Rotation = rot; }
		inline void SetScale(const float scale)  { m_Scale = scale; }

		inline virtual void SetPickablePosition(const glm::vec3& pos) override;
		inline virtual void SetPickableRotation(const glm::vec3& rot) override;
		inline virtual void SetPickableScale(const float scale) override { m_Scale = scale; }

		inline glm::vec3 GetPosition() const { return m_Position; }
		inline glm::vec3 GetRotation() const { return m_Rotation; }
		inline float GetScale() const { return m_Scale; }

		virtual std::pair<glm::vec3, glm::vec3> GetPickablePosition() const override;
		inline virtual glm::vec3 GetPickableRotation() const override { return GetRotation(); }
		inline virtual float GetPickableScale() const override { return GetScale(); }

		inline void SetTransform(const glm::mat4& cacheTransform) { m_Transform = cacheTransform; }
		

		inline glm::mat4 GetTransform() const { return m_Transform; }

		inline glm::vec3 GetOrigin() const { return m_MeshOrigin; }

		AABB GetPickableAABB() const;

		//#NOTE: Like, this is not meant to be a game engine, but It would nice to setup some sort of residency rule to deallocate stuff from gpu memory when it needs
		glm::vec3 m_MeshOrigin = glm::vec3(0.0f);
		glm::vec3 m_HalfWay = glm::vec3(0.0f);
	private:
		void BuildAABB();

		glm::mat4 m_Transform;
		std::vector<uint32_t> m_Indices;
		std::vector<glm::vec3>     m_VertexPositions;
		std::vector<glm::vec3>     m_Normal;
		std::vector<glm::vec3>     m_Tangent;
		std::vector<glm::vec2>     m_TextureCoord;
		
		AABB m_AABB;
		glm::vec3 m_Position = glm::vec3(0.0f), m_Rotation = glm::vec3(0.0f);
		Material* m_Material;
		float m_Scale = 1.0f; 
		bool m_Dirty = false;

	};
}
