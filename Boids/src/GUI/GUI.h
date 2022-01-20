#pragma once

#include "Platform/Window.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace Boids
{

	class GUI
	{
	public:
		static void Init(Window* window);
		static void Begin();
		static void End();
		static void Destroy();
	
	};
}