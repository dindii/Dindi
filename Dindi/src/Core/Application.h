#pragma once
#include "Platform/Window.h"
#include "Utils/Logger.h"
#include "Utils/DeltaTime.h"
#include "Rendering/Scene/Scene.h"
#include "Layer.h"

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
		inline void SetActiveScene(Scene* scene) { m_ActiveScene = scene; }
		float GetTime() const;
		void ProcessEngineInterface();
		void PushLayer(Layer* layer);

		inline void TerminateProgram() { m_Running = false; }

	private:
		static inline Application* s_Instance = nullptr;

		Window* m_ApplicationWindow;
		DeltaTime* m_DeltaTime;

		std::vector<Layer*> m_LayerStack;
	private:
		bool m_Running = true;

	private:
		Scene* m_ActiveScene;
		bool m_MouseLockedAndInvisible = true;
	};
}