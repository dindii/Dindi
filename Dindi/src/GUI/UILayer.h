#pragma once
#include <Rendering/Scene/Scene.h>
#include <Core/Layer.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Dindi
{
	enum EditMode
	{
		BEGIN,
		EditModeMin = 0,

		Translate = EditModeMin,
		Rotation,
		Scale,

		EditModeMax = Scale,
		END
	};

	class UILayer : public Layer
	{
	public:
		UILayer(uint32_t frameWidth, uint32_t frameHeight) : m_FrameWidth(frameWidth), m_FrameHeight(frameHeight) {};
		UILayer() : m_FrameWidth(0), m_FrameHeight(0), m_Scene(nullptr) {};

		virtual void OnUpdate(const Dindi::DeltaTime& dt) override;
		virtual void OnEvent(Dindi::Event& event) override;
		virtual void OnAttach()   override;
		virtual void OnDetach()   override;
		virtual void OnUIRender() override {};

		bool IsHovering() const;

		inline glm::vec2 GetViewportMin() const { return { (float)m_ViewportMinX, (float)m_ViewportMinY }; }
		glm::vec2 GetViewportMousePosition() const;
		glm::vec2 GetViewportSize() const;
		float GetViewportAspectRatio() const { return (float)m_ViewportSizeX / (float)m_ViewportSizeY; }
	private:
		void CacheViewportMinPos(uint32_t width, uint32_t height);
		void CacheViewportSize(uint32_t width, uint32_t height);
		void SetFrameDimensions(uint32_t width, uint32_t height) { m_FrameWidth = width; m_FrameHeight = height; };
		void SetScene(Scene* scene) { m_Scene = scene; }

		void ProcessMenu();
		void ProcessLightInspector();
		void ProcessModelInspector();
		void ProcessTransformGizmo();
		void ProcessViewport();
		void ProcessPerformanceStats(const DeltaTime& dt);
		void ProcessGraphicsDefinitions();

	private:
		uint32_t m_FrameWidth = 0, m_FrameHeight = 0;
		uint32_t m_ViewportSizeX = 1, m_ViewportSizeY = 1;
		uint32_t m_ViewportMinX = 0, m_ViewportMinY = 0;

		Scene* m_Scene = nullptr;
		Model* m_SelectedModel = nullptr;

	private:
		EditMode m_EditMode = EditMode::Translate;
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

		const float m_ViewportPosX = 0.702f;

	};
}