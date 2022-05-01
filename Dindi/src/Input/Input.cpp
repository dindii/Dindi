#pragma once
#include "Dindipch.h"
#include "Input/Input.h"
#include "Core/Application.h"

//#OBS For now, this is made to a single window app, if you decide to add more windows, make a way to specify the window you want
//     to apply these properties (easy stuff, glfw do this for us.)
namespace Dindi
{
	bool Input::IsKeyPressed(EKeyCode kc)
	{
		GLFWwindow* window = Application::GetInstance().GetWindow()->GetNativeWindow();
		auto state = glfwGetKey(window, static_cast<int32_t>(kc));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(MouseButtonsCode mb)
	{
		GLFWwindow* window = Application::GetInstance().GetWindow()->GetNativeWindow();
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(mb));
		return state == GLFW_PRESS;
	}

	void Input::HideAndLockCursor(bool cond)
	{
		GLFWwindow* window = Application::GetInstance().GetWindow()->GetNativeWindow();

		if (cond)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			return;
		}

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}


	Dindi::vec2 Input::GetMousePosition()
	{
		GLFWwindow* window = Application::GetInstance().GetWindow()->GetNativeWindow();
		double xpos = 0, ypos = 0;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	void Input::SetCursorPosition(int x, int y)
	{
		GLFWwindow* window = Application::GetInstance().GetWindow()->GetNativeWindow();
		glfwSetCursorPos(window, x, y);
	}

}