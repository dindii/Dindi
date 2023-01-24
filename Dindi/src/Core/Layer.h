#pragma once
#include "Utils/DeltaTime.h"
#include "Event/Event.h"

namespace Dindi
{
	//I will be using layer for some sort of program. It has all important functions a program needs to run. Then I can create two programs and do different
	//stuff on them. Like a rasterizer program that would use those functions and a raytracer program, I just need to push'em to the application layer stack
	//and they will receive events and they will be updated accordingly. 

	class Layer
	{
	public:
		virtual ~Layer() = default;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;

		virtual void OnUIRender() = 0;
		virtual void OnUpdate(const DeltaTime& dt) = 0;
		virtual void OnEvent(Event& event) = 0;
	};
}