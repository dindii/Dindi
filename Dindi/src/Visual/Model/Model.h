#pragma once
#include "Visual/Material/Material.h"
#include "Visual/Mesh/Mesh.h"
#include <string_view>
#include <Physics/AABB.h>

#include <Interactable/Pickable.h>

namespace Dindi
{
	class Model : public Pickable
	{
	public:
		Model() : m_Scale(1.0f), m_Transform(1.0f), m_Position(0.0f), m_Rotation(0.0f) {};
		Model(std::string modelPath, const glm::vec3& modelPosition, const float modelScale);
		~Model();

		void SetPosition(const glm::vec3& newPos) { m_Position = newPos; }
		inline void SetPickablePosition(const glm::vec3& pos) override { SetPosition(pos); }
		inline void SetPickableRotation(const glm::vec3& rot) override { SetRotation(rot); }
		inline virtual void SetPickableScale(const float scale) override { m_Scale = scale; }

		inline glm::vec3 GetPosition() const { return m_Position; }

		void SetRotation(const glm::vec3& newRot) { m_Rotation = newRot; }
		inline glm::vec3 GetRotation() const { return m_Rotation; }

		void SetScale(const float newScale) { m_Scale = newScale; }
		inline float GetScale() const { return m_Scale; }
		inline std::string_view GetName() { return { m_Name }; }
		void SetName(std::string_view name) { m_Name = name; }

		inline std::vector<Mesh*>& GetMeshes() { return m_Mesh; }
		
		void SortMeshesBasedOnAABBSize(bool greaterThan);

		void AddMesh(Mesh* const newMesh);
		
		void SetWorldAABB(const AABB& aabb) { m_AABB.SetMin(aabb.GetMin()); m_AABB.SetMax(aabb.GetMax()); }
		void SetAABB(const AABB& aabb) { m_AABB = aabb; }
		AABB GetAABB() const { return m_AABB; }
	
		glm::mat4 GetTransform() const { return m_Transform; }

		void BuildAABB();


		virtual std::pair<glm::vec3, glm::vec3> GetPickablePosition() const override;
		inline virtual glm::vec3 GetPickableRotation() const override { return GetRotation(); }
		inline virtual float GetPickableScale() const override { return GetScale(); }

		AABB GetPickableAABB() const;
	private:
		//#OBS We kinda iterate a lot over models and meshes. It would be nice to memory-align and make this struct cache friendly.
		glm::mat4 m_Transform;

		glm::vec3 m_Position, m_Rotation;
		float m_Scale; //#TODO: Non-uniform scale, I'm just lazy to do this right now.
		std::vector<Mesh*> m_Mesh;
		std::string m_Name;
		AABB m_AABB;
	};
}