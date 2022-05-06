#include "Dindipch.h"
#include "Model.h"
#include "Utils/ModelLoader.h"

namespace Dindi
{
	Model::Model(std::string& meshPath, std::string vertexPath /*= DEFAULT_VERTEX_SHADER*/, std::string fragmentPath /*= DEFAULT_FRAGMENT_SHADER*/)
	{
		ModelLoader::Load(meshPath, *this);
	}

	Model::~Model()
	{
		delete m_Material;
		delete m_Mesh;
	}

}