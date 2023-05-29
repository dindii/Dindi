#pragma once
#include "Utils/Logger.h"
#include "Utils/DeltaTime.h"
#include "Rendering/Scene/Scene.h"
#include "Layer.h"
#include <GUI/UILayer.h>

namespace Dindi
{
	class Window;

	class Application
	{
	public:
		Application();
		Application(float windowWidth, float windowHeight, const char* appName = "OpenGL App", float aspectRatio = 1.0f);

	public:
		virtual ~Application();
		void OnEvent(Event& e);
		void Run();
		void OnUpdate(DeltaTime& dt);
		inline Window* GetWindow() { return m_ApplicationWindow; }
		inline static Application& GetInstance() { return *s_Instance; }
		void SetActiveScene(Scene* scene);
		Scene* GetActiveScene() { return m_ActiveScene; }
		float GetTime() const;
		void PushLayer(Layer* layer);



		UILayer* GetUILayer() { return m_UILayer; }

		inline EApplicationState GetApplicationState() const { return m_AppState; }
		inline void TerminateProgram() { m_Running = false; }
	private:
		static inline Application* s_Instance = nullptr;

		Window* m_ApplicationWindow;
		DeltaTime m_DeltaTime;

		UILayer* m_UILayer;

		std::vector<Layer*> m_LayerStack;
	private:
		bool m_Running = true;

	private:
		Scene* m_ActiveScene;
		bool m_MouseLockedAndInvisible = true;
		bool m_AppInitialized = false;

		EApplicationState m_AppState;
	};
}