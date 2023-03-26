#pragma once
#include "Visual/Material/Material.h"
#include "Visual/Mesh/Mesh.h"
#include <string_view>
#include <Physics/AABB.h>

namespace Dindi
{
	class Model
	{
	public:
		Model() : m_Scale(0.0f), m_Dirty(false) {};
		Model(std::string modelPath, const vec3& modelPosition, const float modelScale);
		~Model();

		void SetPosition(const vec3& newPos) { m_Position = newPos; }
		inline vec3 GetPosition() const { return m_Position; }

		void SetRotation(const vec3& newRot) { m_Rotation = newRot; }
		inline vec3 GetRotation() const { return m_Rotation; }

		void SetScale(const float newScale) { m_Scale = newScale; }
		inline float GetScale() const { return m_Scale; }
		inline std::string_view GetName() { return { m_Name }; }
		void SetName(std::string_view name) { m_Name = name; }

		inline std::vector<Mesh*>& GetMeshes() { return m_Mesh; }
		
		void SortMeshesBasedOnAABBSize(bool greaterThan);

		inline void AddMesh(Mesh* const newMesh) { m_Mesh.emplace_back(newMesh); };
		
		void SetWorldAABB(const AABB& aabb) { m_AABB.SetMin(aabb.GetMin()); m_AABB.SetMax(aabb.GetMax()); }
		void SetAABB(const AABB& aabb) { m_AABB = aabb; }
		AABB GetAABB() const { return m_AABB; }
		void SetTransform(const mat4& transform);
		mat4 GetTransform() const { return m_Transform; }

		inline void SetDirty(bool dirty) { m_Dirty = dirty; }
		inline bool GetDirty() const { return m_Dirty; }

		void BuildAABB(bool setLocalSpace = false);
	private:
		//#OBS We kinda iterate a lot over models and meshes. It would be nice to memory-align and make this struct cache friendly.
		mat4 m_Transform;

		vec3 m_Position, m_Rotation;
		float m_Scale; //#TODO: Non-uniform scale, I'm just lazy to do this right now.
		std::vector<Mesh*> m_Mesh;
		std::string m_Name;
		AABB m_AABB;
		bool m_Dirty;
	};
}