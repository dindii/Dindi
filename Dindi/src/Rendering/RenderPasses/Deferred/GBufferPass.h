#pragma once
#include "Rendering/RenderPasses/RenderPass.h"

namespace Dindi
{
	class GBufferRenderPass : public RenderPass
	{
	public:
		GBufferRenderPass();
		~GBufferRenderPass();

		virtual void GenerateOutput(Scene* scene) override final;
		std::vector<Texture2D*> GetGbufferResources() { return m_GBuffer; }
	protected:
		virtual void TransformAndDraw(Scene* scene) override final;
		void TransformAndDrawDebugShapes(Scene* scene);
	private:
		std::vector<Texture2D*> m_GBuffer;
	};
}