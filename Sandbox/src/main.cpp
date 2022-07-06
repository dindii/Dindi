#include "Dindipch.h"
#include <Core/Application.h>
#include "RasterizerExample.h"

int main()
{
	Dindi::Application app(800, 800, "Janela uau", 4.0f / 3.0f);

	RasterizerExample* rasterizerProgram = new RasterizerExample();
	app.PushLayer(rasterizerProgram);

	app.Run();
}