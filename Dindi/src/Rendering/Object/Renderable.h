#pragma once

#include <vector>

#include "Math/vec3.h"
#include "Math/vec2.h"
#include "Rendering/Shader/Shader.h"

#include <Core/Core.h>

namespace Dindi
{
	enum class RenderFlags : uint32_t
	{
		None    = BIT(0),
		Static  = BIT(1),
		Stream	= BIT(2),
		Dynamic	= BIT(3)
	};

	inline RenderFlags operator|(const RenderFlags firstFlag, const RenderFlags secondFlag) { return (RenderFlags)((uint32_t)firstFlag | (uint32_t)secondFlag);};
	inline RenderFlags operator|=(RenderFlags& firstFlag, const RenderFlags secondFlag) { return (RenderFlags&)((uint32_t&)firstFlag |= (uint32_t)secondFlag);};

	static bool operator&(const RenderFlags firstFlag, const RenderFlags secondFlag) { return ((uint32_t)firstFlag & (uint32_t)secondFlag) > 0;};

	class Renderable
	{
	public:
		//TEMPORARY, BEFORE ABSTRACTION
		Renderable();
	
		unsigned int virtual GetVertexBufferObjectID() const { return m_VertexBufferObjectID; }
		unsigned int virtual GetVertexArrayObjectID()  const { return m_VertexArrayObjectID; }
		unsigned int virtual GetElementsCount() const { return m_Elements; }

		inline void AppendRenderFlag(const RenderFlags flag) { m_Flags |= flag; }
		inline void SetRenderFlag(const RenderFlags flag) { m_Flags = flag; }

	protected:
		void RegisterMeshData(std::vector<vec3>& vertexPositions, std::vector<vec3>& vertexNormals, std::vector<vec2>& textCoords);
	private:
		//this will have a vbo abstraction
		unsigned int m_VertexBufferObjectID, m_VertexArrayObjectID;
		
		// this will be dynamic and not for a triangle
		int m_Elements;
		RenderFlags m_Flags = RenderFlags::Static;
	};
}