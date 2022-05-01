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
  IncludeDir["assimp"] = "Dindi/vendor/assimp"

  group "Dependencies"
  include "Dindi/vendor/Glad"
  include "Dindi/vendor/imgui"
  include "Dindi/vendor/glfw"
  include "Dindi/vendor/assimp"
  group ""

project "Dindi"
	location "Dindi"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

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
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
		"%{IncludeDir.stbi}",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.assimp}"
	}
	
	links
	{
		"Glad",
        "ImGui",
		"GLFW",
		"assimp",
        "opengl32.lib"
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
	defines "DINDI_RELEASE"
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
		"Dindi/src",
        "Dindi/vendor"
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
	defines "DINDI_RELEASE"
	runtime "Release"
	optimize "Debug"

	filter "configurations:Dist"
	defines "DINDI_DIST"
	runtime "Release"
	symbols "Off"
	optimize "Full"
	
	
