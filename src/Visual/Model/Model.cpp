#include "Model.h"

namespace GLR
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