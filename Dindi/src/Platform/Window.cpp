#include "Dindipch.h"
#include "Window.h"
#include "Utils/Logger.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "Event/ApplicationEvent.h"


namespace Dindi
{
	Window::Window(float width, float height, const char* m_Name, float aspectRatio)
	{
		if (width < 0 || height < 0)
			DND_LOG_FATAL("Could not create window: negative resolution.");

		if (!glfwInit())
			DND_LOG_FATAL("Could not initialize window system!");

		if (aspectRatio <= 0.0f)
			DND_LOG_WARNING("Invalid aspect ratio input.");
		
		m_AspectRatio = aspectRatio;
		
		m_Width = width;
		m_Height = (float)height / aspectRatio;

		m_Window = glfwCreateWindow((int)width, (int)(height), m_Name, NULL, NULL);


		if(!m_Window)
			DND_LOG_FATAL("Could not create window!");

		glfwSetWindowUserPointer(m_Window, this);

		//For any graphics api
		glfwMakeContextCurrent(m_Window);
	//	glfwSwapInterval(0);


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
			Window* ourWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			ourWindow->cursorPositionCallback(window, xpos, ypos);

			//caching mouse positions
			ourWindow->SetCacheMousePos(xpos, ypos);
		};

		
	
		glfwSetCursorPosCallback(m_Window, WindowMouseMovedCallback);

		//#TODO: This AA is temporary, I guess I will not use MSAA, but please put this 4 to a config file, maybe. 
		//#WARNING: check if this will make my life harder with the g-buffer.
		glfwWindowHint(GLFW_SAMPLES, 4);
	}
	
	Window::~Window()
	{
	}
	
	void Window::OnUpdate()
	{
		swapBuffers();
		pollEvents();
	}
	
	float Window::GetAspectRatio() const
	{
		//#WARNING This is kinda dumb because of precision issues (that I don't think we will have), please get a better way of doing this.
		if (m_AspectRatio == 1.0f)
			return (float)m_Width / (float)m_Height;

		return m_AspectRatio;
	}

	float Window::GetTime() const
	{
		return (float)glfwGetTime();
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
		MouseMovedEvent e((uint32_t)xpos, (uint32_t)ypos);
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
		m_Width = width;
		m_Height = height;

		WindowResizeEvent e(width, height);
		EventCallback(e);
	}

	void Window::windowCloseCallback(GLFWwindow* window)
	{
		WindowCloseEvent e;
		EventCallback(e);
	}

}
