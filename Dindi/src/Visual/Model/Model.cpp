#include "Dindipch.h"
#include "Model.h"

namespace Dindi
{
	Model::Model(std::string& modelPath)
	{

	}

	Model::~Model()
	{
		delete m_Material;
		delete m_Mesh;
	}

}