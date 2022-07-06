#pragma once
#include "Visual/Material/Material.h"
#include "Visual/Mesh/Mesh.h"

namespace Dindi
{
	class Model
	{
	public:
		Model() : m_Mesh(nullptr), m_Material(nullptr) {};

		Model(std::string meshPath, std::string vertexPath = DEFAULT_VERTEX_SHADER, std::string fragmentPath = DEFAULT_FRAGMENT_SHADER);
		~Model();

		inline Mesh* GetMesh() { return m_Mesh; }
		inline Material* GetMaterial() { return m_Material; }

	private:
		Mesh* m_Mesh;
		Material* m_Material;
	};
}