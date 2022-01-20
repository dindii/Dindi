#include "boidspch.h"
#include "Model.h"

namespace Boids
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