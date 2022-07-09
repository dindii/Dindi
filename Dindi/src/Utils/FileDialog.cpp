#include "Dindipch.h"
#include "FileDialog.h"

#ifdef DINDI_PLATFORM_WINDOWS
#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include "Core/Application.h"
#include <GLFW/glfw3native.h>


namespace Dindi
{
	//#NOTE: To be honest, this should be better with some multiplataform architecture but I guess I will only use Windows... 

	std::string FileDialog::OpenFile(const char* filter)
	{
#ifdef DINDI_PLATFORM_WINDOWS
		OPENFILENAMEA ofn;
		CHAR szFile[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(Application::GetInstance().GetWindow()->GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		
		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
#endif

		return std::string();
	}

}