#pragma once
#include "Event/Event.h"
#include "Core/Application.h"
#include "Event/Event.h"

struct GLFWwindow;

namespace Dindi
{
	class Window
	{
		using EventCallbackFn = std::function<void(Event&)>;

	public:
		Window(float width, float height, const char* m_Name, float aspectRatio);
	
		virtual ~Window();
	
		void OnUpdate();
	
		inline uint32_t GetWidth()  const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height;}
		inline const char* GetName() const { return m_Name; }
		inline GLFWwindow* GetNativeWindow() { return m_Window; }
		inline float GetAspectRatio() const { return m_AspectRatio; }
	
		inline void setEventCallback(const EventCallbackFn& callback) { EventCallback = callback; }

	
	private:
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
	};
}