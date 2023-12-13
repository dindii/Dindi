#include "Dindipch.h"
#include "Model.h"
#include "Utils/ModelLoader.h"
#include "Utils/DNDAssert.h"
#include <algorithm>

namespace Dindi
{
	//I will not take meshPath by reference because we can just use the root asset path macro with the actual asset file easier this way.
	//No difference between RESOURCES_PATH + std::string(path) and  RESOURCES_PATH path, the later we can have more simplicity.
	Model::Model(std::string meshPath, const glm::vec3& modelPosition, const float modelScale) :
		m_Position(modelPosition), m_Scale(modelScale), m_Transform(1.0f), m_Rotation(0.0f)
	{								   
		//#NOTE: Not sure if I want to allocate memory here but RAII would mess me up.
		//m_Mesh = new Mesh();
		//m_Material = new Material(vertexPath, fragmentPath);
		ModelLoader::Load(meshPath, *this);
		
		BuildAABB();
		
		//#TODO - NOW - We have to create a material for each mesh so the loader can register a texture in this material. 
		//Probably we will create in the runtime and assign its maps. This all will be by default.
		//If the user want to assign a different material/shader to a mesh, it can simply use some sort of SetMaterial() function
		//So by default, we will just create materials with the default shaders and using the textures of the model

		//#TODO:Use the RegisterData on the low level rendering and inside itself.
		//Register loaded data layout. (Vertex positions, normals, texture coords etc on the shader.
		for (uint32_t x = 0; x < m_Mesh.size(); x++)
		{
			DND_ASSERT(m_Mesh[x], "The mesh is invalid, but we just created the mesh, check this!");
			
			if(m_Mesh[x])
				m_Mesh[x]->RegisterData();
		}

		SortMeshesBasedOnAABBSize(false);
	}

	Model::~Model()
	{
		for (uint32_t x = 0; x < m_Mesh.size(); x++)
		{
			if (m_Mesh[x])
				delete m_Mesh[x];
		}
	}

	void Model::SortMeshesBasedOnAABBSize(bool greaterThan)
	{
		if(greaterThan)
			std::sort(m_Mesh.begin(), m_Mesh.end(), [](Mesh* a, Mesh* b) { return a->GetAABB().GetAABBSize() > b->GetAABB().GetAABBSize(); });
		else
			std::sort(m_Mesh.begin(), m_Mesh.end(), [](Mesh* a, Mesh* b) { return a->GetAABB().GetAABBSize() < b->GetAABB().GetAABBSize(); });
	}

	void Model::BuildAABB()
	{
		AABB meshAABB = m_Mesh[0]->GetAABB();

		glm::vec3 minBoundaries = meshAABB.GetMin(), maxBoundaries = meshAABB.GetMax();

		for (uint32_t i = 0; i < m_Mesh.size(); i++)
		{
			AABB meshAABB = m_Mesh[i]->GetAABB();

			glm::vec3 meshMinAABB = meshAABB.GetMin();
			

			if (meshMinAABB.x <= minBoundaries.x)
				minBoundaries.x = meshMinAABB.x;

			if (meshMinAABB.y <= minBoundaries.y)
				minBoundaries.y = meshMinAABB.y;

			if (meshMinAABB.z <= minBoundaries.z)
				minBoundaries.z = meshMinAABB.z;
			

			glm::vec3 meshMaxAABB = meshAABB.GetMax();

			if (meshMaxAABB.x >= maxBoundaries.x)
				maxBoundaries.x = meshMaxAABB.x;

			if (meshMaxAABB.y >= maxBoundaries.y)
				maxBoundaries.y = meshMaxAABB.y;

			if (meshMaxAABB.z >= maxBoundaries.z)
				maxBoundaries.z = meshMaxAABB.z;
		}

		m_AABB = { minBoundaries, maxBoundaries };
	}

	std::pair<glm::vec3, glm::vec3> Model::GetPickablePosition() const
	{
		return { std::make_pair(m_Position, glm::vec3(0.0f)) };
	}

	Dindi::AABB Model::GetPickableAABB() const
	{
		return GetAABB();
	}

}