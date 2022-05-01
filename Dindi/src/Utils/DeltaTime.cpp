#include "Dindipch.h"
#include "DeltaTime.h"
#include <Windows.h>

#include <GLFW/glfw3.h>
#include "Utils/Logger.h"

namespace Dindi
{
	DeltaTime::DeltaTime() : currentTime(0), previousTime(0), deltaTime(0.0f), secondsPerCount(0.0f)
	{
		previousTime = glfwGetTime();
	}

	void DeltaTime::Tick()
	{	
		currentTime = glfwGetTime();
		deltaTime = currentTime - previousTime;
		previousTime = currentTime;
	}	

	double DeltaTime::getDeltaTime()
	{
		return deltaTime;
	}

}