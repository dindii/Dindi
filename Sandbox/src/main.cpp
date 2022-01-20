#include "boidspch.h"
#include <Core/Application.h>

int main()
{
	Boids::Application app(800, 800, "Janela uau", 4.0f / 3.0f);
	app.Run();
}