#pragma once
#include "Visual/Material/Material.h"
#include "Visual/Mesh/Mesh.h"

namespace GLR
{
	class Model
	{
	public:
		//#TODO: check for duplicates
		Model(std::string& modelPath);
		Model(Mesh* mesh, Material* material) : m_Mesh(mesh), m_Material(material) {};
		
		~Model();

		inline Mesh* GetMesh() { return m_Mesh; }
		inline Material* GetMaterial() { return m_Material; }

	private:
		Mesh* m_Mesh;
		Material* m_Material;
	};
}