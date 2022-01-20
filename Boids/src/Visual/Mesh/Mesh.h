#pragma once
#include "Rendering/Object/Renderable.h"

namespace Boids
{
	class Mesh : public Renderable
	{
	public:
		//We will have something here to init
		//We'll extent this or add a copy constructor soon
		Mesh(const std::vector<vec3>& vertexPositions);
		//this will be dynamic as well
	private:
		std::vector<vec3> m_VertexPositions;
	};
}
