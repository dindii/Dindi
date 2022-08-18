#include "Dindipch.h"
#include "Model.h"
#include "Utils/ModelLoader.h"

namespace Dindi
{
	//I will not take meshPath by reference because we can just use the root asset path macro with the actual asset file easier this way.
	//No difference between RESOURCES_PATH + std::string(path) and  RESOURCES_PATH path, the later we can have more simplicity.
	Model::Model(std::string meshPath, const vec3& modelPosition, const float modelScale, std::string vertexPath, std::string fragmentPath) :
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

		//Register loaded data layout. (Vertex positions, normals, texture coords etc on the shader.
		m_Mesh->RegisterData();
	}

	Model::Model(std::vector<vec3>& vertex, std::string vertexPath, std::string fragmentPath) : m_Scale(1.0f)
	{
		m_Mesh = new Mesh(std::move(vertex));
		m_Material = new Material(vertexPath, fragmentPath);
		m_Mesh->RegisterData();
	}

	Model::~Model()
	{
		delete m_Mesh;
		delete m_Material;
	}

}