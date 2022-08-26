#include "Dindipch.h"
#include "Model.h"
#include "Utils/ModelLoader.h"
#include "Utils/DNDAssert.h"

namespace Dindi
{
	//I will not take meshPath by reference because we can just use the root asset path macro with the actual asset file easier this way.
	//No difference between RESOURCES_PATH + std::string(path) and  RESOURCES_PATH path, the later we can have more simplicity.
	Model::Model(std::string meshPath, const vec3& modelPosition, const float modelScale) :
		m_Position(modelPosition), m_Scale(modelScale)
	{								   
		//#NOTE: Not sure if I want to allocate memory here but RAII would mess me up.
		//m_Mesh = new Mesh();
		//m_Material = new Material(vertexPath, fragmentPath);
		ModelLoader::Load(meshPath, *this);
		
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
	}

	Model::~Model()
	{
		for (uint32_t x = 0; x < m_Mesh.size(); x++)
		{
			if (m_Mesh[x])
				delete m_Mesh[x];
		}
	}

}