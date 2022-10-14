#pragma once
#include "Visual/Material/Material.h"
#include "Visual/Mesh/Mesh.h"
#include <string_view>

namespace Dindi
{
	class Model
	{
	public:
		Model() : m_Scale(0.0f) {};
		Model(std::string modelPath, const vec3& modelPosition, const float modelScale);
		~Model();

		void SetPosition(const vec3& newPos) { m_Position = newPos; }
		inline vec3 GetPosition() const { return m_Position; }

		void SetScale(const float newScale) { m_Scale = newScale; }
		inline float GetScale() const { return m_Scale; }
		inline std::string_view GetName() { return { m_Name }; }
		void SetName(std::string_view name) { m_Name = name; }

		inline std::vector<Mesh*>& GetMeshes() { return m_Mesh; }
		inline void AddMesh(Mesh* const newMesh) { m_Mesh.emplace_back(newMesh); };
		

	private:
		vec3 m_Position;
		float m_Scale; //#TODO: Non-uniform scale, I'm just lazy to do this right now.
		std::vector<Mesh*> m_Mesh;
		std::string m_Name;
	};
}