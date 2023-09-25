#pragma once
#include "Event/Event.h"
#include "Core/Application.h"
#include <GLFW/glfw3.h>

namespace Dindi
{
	class Window
	{
		using EventCallbackFn = std::function<void(Event&)>;

	public:
		Window(float width, float height, const char* m_Name, float aspectRatio = 1.0f);
	
		virtual ~Window();
	
		void OnUpdate();
	
		inline uint32_t GetWidth()  const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height;}
		inline const char* GetName() const { return m_Name; }
		inline GLFWwindow* GetNativeWindow() { return m_Window; }
		float GetAspectRatio() const;
		float GetTime() const;
		inline void setEventCallback(const EventCallbackFn& callback) { EventCallback = callback; }
	
		inline uint32_t GetMouseX() const { return m_MouseX; }
		inline uint32_t GetMouseY() const { return m_MouseY; }
		inline glm::vec2 GetDimensions() const { return { (float)m_Width, (float)m_Height }; }
	private:
		inline void SetCacheMousePos(uint32_t x, uint32_t y) { m_MouseX = x; m_MouseY = y; }

		void pollEvents(); 
		void swapBuffers();

		void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
		void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		void windowResizeCallback(GLFWwindow* window, int width, int height);
		void windowCloseCallback(GLFWwindow* window);

		EventCallbackFn EventCallback;

		GLFWwindow* m_Window;
		const char* m_Name;
		float m_AspectRatio;
		uint32_t m_Width, m_Height;
		uint32_t m_MouseX = 0, m_MouseY = 0;
	};
}