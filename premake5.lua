workspace "Dindi"
		architecture "x64"
		startproject "SandBox"


configurations
{
	"Debug",
	"Release",
	"Dist"
}

characterset ("MBCS")

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

 IncludeDir = {}
  IncludeDir["Glad"] = "Dindi/vendor/Glad/include"
  IncludeDir["ImGui"] = "Dindi/vendor/imgui"
  IncludeDir["stbi"] = "Dindi/vendor/stbi"
  IncludeDir["glfw"] = "Dindi/vendor/glfw/include"

  group "Dependencies"
  include "Dindi/vendor/Glad"
  include "Dindi/vendor/imgui"
  include "Dindi/vendor/glfw"
  group ""

project "Dindi"
	location "Dindi"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	toolset "v141"  

	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "Dindipch.h"
	pchsource "Dindi/src/Dindipch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/imgui/backends/imgui_impl_glfw.cpp",
		"%{prj.name}/vendor/imgui/backends/imgui_impl_glfw.h",

		"%{prj.name}/vendor/imgui/backends/imgui_impl_opengl3.cpp",
		"%{prj.name}/vendor/imgui/backends/imgui_impl_opengl3.h",
		
		"%{prj.name}/vendor/imguizmo/ImGuizmo.cpp",
		"%{prj.name}/vendor/imguizmo/ImGuizmo.h",

	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/imguizmo",

		"%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
		"%{IncludeDir.stbi}",
		"%{IncludeDir.glfw}"
	}
	
	links
	{
		"Glad",
        "ImGui",
		"GLFW"
	}

	filter "system:windows"
	systemversion "latest"

	defines
	{
		"DINDI_PLATFORM_WINDOWS"
	}

	filter "configurations:Debug"
	defines "DINDI_DEBUG"
	runtime "Debug"
	symbols "on"

	filter "configurations:Release"
	defines "DINDI_DEBUG"
	runtime "Release"
	optimize "Debug"

	filter "configurations:Dist"
	defines "DINDI_DIST"
	runtime "Release"
	symbols "Off"
	optimize "Full"



	project "SandBox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	toolset "v141"  

	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"../",
		"Dindi/src",
        "Dindi/vendor",
        "Dindi/vendor/glfw/include"
	}

	links                      -- Linkamos a Engine no Sandbox também
      {
      	 "Dindi"
      }
	
	filter "system:windows"
	systemversion "latest"

	defines
	{
		"DINDI_PLATFORM_WINDOWS"
	}

	filter "configurations:Debug"
	defines "DINDI_DEBUG"
	runtime "Debug"
	symbols "on"

	filter "configurations:Release"
	defines "DINDI_DEBUG"
	runtime "Release"
	optimize "Debug"

	filter "configurations:Dist"
	defines "DINDI_DIST"
	runtime "Release"
	symbols "Off"
	optimize "Full"
	
	
