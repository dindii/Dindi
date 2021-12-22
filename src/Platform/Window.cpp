#include "Window.h"
#include "Utils/Logger.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "Event/ApplicationEvent.h"
#include <GLFW/glfw3.h>

namespace GLR
{
	Window::Window(float width, float height, const char* m_Name, float aspectRatio)
	{
		if (width < 0 || height < 0)
			GLR_LOG_FATAL("Could not create window: negative resolution.");

		if (!glfwInit())
			GLR_LOG_FATAL("Could not initialize window system!");

		if (aspectRatio <= 0.0f)
			GLR_LOG_WARNING("Invalid aspect ratio input.");
			
		m_AspectRatio = aspectRatio;

		m_Window = glfwCreateWindow(width, int((height / aspectRatio)), m_Name, NULL, NULL);

		if(!m_Window)
			GLR_LOG_FATAL("Could not create window!");

		glfwSetWindowUserPointer(m_Window, this);

		//For any graphics api
		glfwMakeContextCurrent(m_Window);


		//Set key callback 
		auto KeyCallbackFunc = [](GLFWwindow* window, int key, int scanCode, int action, int mods)
		{
			static_cast<Window*>(glfwGetWindowUserPointer(window))->keyCallback(window, key, scanCode, action, mods);
		};

		glfwSetKeyCallback(m_Window, KeyCallbackFunc);

		//Set mouse button callback
		auto MouseButtonCallbackFunc = [](GLFWwindow* window, int button, int action, int mods)
		{
			static_cast<Window*>(glfwGetWindowUserPointer(window))->mouseButtonCallback(window, button, action, mods);
		};

		glfwSetMouseButtonCallback(m_Window, MouseButtonCallbackFunc);


		//Set window resize callback
		auto WindowResizeCallbackFunc = [](GLFWwindow* window, int width, int height)
		{
			static_cast<Window*>(glfwGetWindowUserPointer(window))->windowResizeCallback(window, width, height);
		};

		glfwSetWindowSizeCallback(m_Window, WindowResizeCallbackFunc);


		//Set window close callback
		auto WindowCloseCallbackFunc = [](GLFWwindow* window)
		{
			static_cast<Window*>(glfwGetWindowUserPointer(window))->windowCloseCallback(window);
		};

		glfwSetWindowCloseCallback(m_Window, WindowCloseCallbackFunc);


		//Set window mouse moved callback
		auto WindowMouseMovedCallback = [](GLFWwindow* window, double xpos, double ypos)
		{
			static_cast<Window*>(glfwGetWindowUserPointer(window))->cursorPositionCallback(window, xpos, ypos);
		};
	
		glfwSetCursorPosCallback(m_Window, WindowMouseMovedCallback);

	}
	
	Window::~Window()
	{
	}
	
	void Window::onUpdate()
	{
		swapBuffers();
		pollEvents();
	}
	
	void Window::pollEvents()
	{
		glfwPollEvents();
	}
	
	void Window::swapBuffers()
	{
		glfwSwapBuffers(m_Window);
	}


	void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		switch (action)
		{
			case GLFW_PRESS:
			{
				KeyPressedEvent e(key, false);
				EventCallback(e);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent e(key, true);
				EventCallback(e);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent e(key);
				EventCallback(e);
			}
		}

	}

	void Window::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
	{
		MouseMovedEvent e(xpos, ypos);
		EventCallback(e);
	}

	void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		switch (action)
		{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent e(button);
				EventCallback(e);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent e(button);
				EventCallback(e);
				break;
			}
		}
	}

	void Window::windowResizeCallback(GLFWwindow* window, int width, int height)
	{
		WindowResizeEvent e(width, height);
		EventCallback(e);
	}

	void Window::windowCloseCallback(GLFWwindow* window)
	{
		WindowCloseEvent e;
		EventCallback(e);
	}

}
