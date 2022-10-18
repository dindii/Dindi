#pragma once

#include "Platform/Window.h"

#include "imgui.h"
#include "imguizmo.h"

namespace Dindi
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