#pragma once
#include "Platform/Window.h"
#include "Utils/Logger.h"
#include "Utils/DeltaTime.h"

#include "Rendering/Scene/Scene.h"


namespace Boids
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

		void ProcessEngineInterface();

		inline void TerminateProgram() { m_Running = false; }

	private:
		static inline Application* s_Instance = nullptr;

		Logger* m_Logger;
		Window* m_ApplicationWindow;
		DeltaTime* m_DeltaTime;
	private:
		bool m_Running = true;

	private:
		Scene* m_ActiveScene;
		Scene* SceneOne;

		Camera* m_DefaultEditorCamera;
		float m_DefaultEditorCameraSpeed;

		bool m_MouseLockedAndInvisible = true;
		//Editor specific stuff
	private:
		void MoveCamera(DeltaTime& dt);
		void LookAround();
	};
}