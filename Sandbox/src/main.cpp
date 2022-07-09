#include "Dindipch.h"
#include <Core/Application.h>
#include "RasterizerExample.h"

int main()
{
	Dindi::Application app(1366, 768, "Janela uau");

	RasterizerExample* rasterizerProgram = new RasterizerExample();
	app.PushLayer(rasterizerProgram);

	app.Run();
}