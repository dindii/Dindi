#pragma once
#include "Visual/Material/Material.h"
#include "Visual/Mesh/Mesh.h"

namespace Dindi
{
	class Model
	{
	public:
		Model() : m_Mesh(nullptr), m_Material(nullptr), m_Scale(0.0f) {};

		Model(std::string meshPath, const vec3& modelPosition, const float modelScale, std::string vertexPath = DEFAULT_VERTEX_SHADER, std::string fragmentPath = DEFAULT_FRAGMENT_SHADER);
		Model(std::vector<vec3>& vertex, std::string vertexPath = DEFAULT_VERTEX_SHADER, std::string fragmentPath = DEFAULT_FRAGMENT_SHADER);

		void SetPosition(const vec3& newPos) { m_Position = newPos; }
		inline vec3 GetPosition() const { return m_Position; }

		void SetScale(const float newScale) { m_Scale = newScale; }
		inline float GetScale() const { return m_Scale; }

		~Model();

		inline Mesh* GetMesh() { return m_Mesh; }
		inline Material* GetMaterial() { return m_Material; }

	private:
		vec3 m_Position;
		float m_Scale; //#TODO: Non-uniform scale, I'm just lazy to do this right now.
		
		Mesh* m_Mesh;
		Material* m_Material;
	};
}