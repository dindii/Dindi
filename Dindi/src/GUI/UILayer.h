#pragma once
#include <Rendering/Scene/Scene.h>

namespace Dindi
{
	class UILayer
	{
	public:
		UILayer(uint32_t frameWidth, uint32_t frameHeight) : m_FrameWidth(frameWidth), m_FrameHeight(frameHeight) {};
		UILayer() : m_FrameWidth(0), m_FrameHeight(0), m_Scene(nullptr) {};

		void SetFrameDimensions(uint32_t width, uint32_t height) { m_FrameWidth = width; m_FrameHeight = height; };
		void SetScene(Scene* scene) { m_Scene = scene; }
		

		void Update(const DeltaTime& dt);
	private:
		void ProcessMenu();
		void ProcessLightInspector();
		void ProcessModelInspector();
		void ProcessPerformanceStats(const DeltaTime& dt);

	private:
		uint32_t m_FrameWidth = 0, m_FrameHeight = 0;
		Scene* m_Scene = nullptr;
	};
}