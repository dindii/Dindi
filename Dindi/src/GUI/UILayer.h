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
		void ProcessTransformGizmo();
		void ProcessPerformanceStats(const DeltaTime& dt);

	private:
		uint32_t m_FrameWidth = 0, m_FrameHeight = 0;
		
		Scene* m_Scene = nullptr;
		Model* m_SelectedModel = nullptr;

	private:
		//Windows raw dimensions and ratios to be calculated with the window size.
		//#TODO: Check if I will have to turn those >1.0f dimensions into ratio (screen percentage) dimensions

		const float m_MenuBarHeight = 22.0f;
		
		float m_PositionSliderSpeed = 0.05f, m_ScaleSliderSpeed = 0.01f;
		
		const float m_StatsWindowHeight = 65.0f, m_StatsWindowWidth = 0.07f;
		const float m_StatsWindowPosX = 0.78f;
		
		const float m_LightInspectorPosX = 0.85f;
		const float m_LightInspectorWidth = 0.15f;

		const float m_ModelInspectorWindowPosX = 0.0f;
		const float m_ModelInspectorWindowWidth = 0.15f;
	};
}