workspace "Boids"
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
  IncludeDir["Glad"] = "Boids/vendor/Glad/include"
  IncludeDir["ImGui"] = "Boids/vendor/imgui"
  IncludeDir["stbi"] = "Boids/vendor/stbi"
  IncludeDir["glfw"] = "Boids/vendor/glfw/include"
  IncludeDir["assimp"] = "Boids/vendor/assimp"

  group "Dependencies"
  include "Boids/vendor/Glad"
  include "Boids/vendor/imgui"
  include "Boids/vendor/glfw"
  include "Boids/vendor/assimp"
  group ""

project "Boids"
	location "Boids"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "boidspch.h"
	pchsource "Boids/src/boidspch.cpp"

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
		"BOIDS_PLATFORM_WINDOWS"
	}

	filter "configurations:Debug"
	defines "BOIDS_DEBUG"
	runtime "Debug"
	symbols "on"

	filter "configurations:Release"
	defines "BOIDS_RELEASE"
	runtime "Release"
	optimize "Debug"

	filter "configurations:Dist"
	defines "BOIDS_DIST"
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
		"Boids/src",
        "Boids/vendor"
	}

	links                      -- Linkamos a Engine no Sandbox também
      {
      	 "Boids"
      }
	
	filter "system:windows"
	systemversion "latest"

	defines
	{
		"BOIDS_PLATFORM_WINDOWS"
	}

	filter "configurations:Debug"
	defines "BOIDS_DEBUG"
	runtime "Debug"
	symbols "on"

	filter "configurations:Release"
	defines "BOIDS_RELEASE"
	runtime "Release"
	optimize "Debug"

	filter "configurations:Dist"
	defines "BOIDS_DIST"
	runtime "Release"
	symbols "Off"
	optimize "Full"
	
	
