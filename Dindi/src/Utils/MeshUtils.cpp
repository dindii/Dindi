#include "Dindipch.h"
#include "MeshUtils.h"

namespace Dindi
{
	static float quadVertices[] = {
	-1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	 1.0f,  1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f
	};

	static float quadUVs[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	Model* MeshUtil::GenerateQuad(const std::string& vertexpath, const std::string& fragmentPath)
	 {
		Mesh* quadMesh = new Mesh(vertexpath, fragmentPath);
		Model* quadModel = new Model();

		std::vector<glm::vec3> quadVerticesMem;
		std::vector<glm::vec2> quadUVsMem;

		quadVerticesMem.resize(4);
		quadUVsMem.resize(4);

		memcpy(&quadVerticesMem[0], quadVertices, 4 * sizeof(glm::vec3));
		memcpy(&quadUVsMem[0], quadUVs, 4 * sizeof(glm::vec2));
		
		quadMesh->SetVertexPositionData(std::move(quadVerticesMem));
		quadMesh->SetTextureCoordData(std::move(quadUVsMem));
		
		quadModel->AddMesh(quadMesh);

		return quadModel;
	 }

}