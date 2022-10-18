#include "Dindipch.h"
#include "GUI.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "styles/ImguiCustomStyle.h"

namespace Dindi
{
	void GUI::Init(Window * window)
	{
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();

		ImGui_ImplGlfw_InitForOpenGL(window->GetNativeWindow(), true);
		const char* glsl_version = "#version 330";
		ImGui_ImplOpenGL3_Init(glsl_version);

		ImGui::LoadStyle();
	}

	void GUI::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void GUI::End()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	void GUI::Destroy()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
		ImGui_ImplGlfw_Shutdown();
	}
}