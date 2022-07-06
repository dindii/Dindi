#pragma once
#include "Dindipch.h"
#include "DebugRenderer.h"
#include "Visual/Model/Model.h"

#include <glad/glad.h>
namespace Dindi
{
	Model DebugRenderer::m_Cube;

	void DebugRenderer::Init()
	{
		//m_Cube = Model(RESOURCES_PATH "Resources/Models/cube.obj");
	}

	void DebugRenderer::DrawCube(vec3 pos, vec3 color)
	{
		//glDrawArrays(GL_TRIANGLES, 0, m_Cube.GetMesh()->GetVertexCount());
	}

}