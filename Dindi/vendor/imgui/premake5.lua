project "ImGui"
    kind "StaticLib"
    language "C++"
	staticruntime "on"
    toolset "v141"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "imconfig.h",
        "imgui.h",
        "imgui.cpp",
        "imgui_draw.cpp",
        "imgui_internal.h",
        "imgui_widgets.cpp",
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h",
        "imgui_demo.cpp",
        "imgui_tables.cpp",
        "backends/imgui_impl_glfw.h",
        "backends/imgui_impl_opengl3.h"
    }

    includedirs
    {
     "imgui"
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
        symbols "on"               -- Deixamos on os debug symbols
        
     filter "configurations:Release"
	 	defines "DINDI_DEBUG"
        runtime "Release"
        optimize "on"            -- Ripamos todas as configurações de debug e otimizamos o projeto

     filter "configurations:Dist"
	 defines "DINDI_DIST"
	 runtime "Release"
	 symbols "Off"
	 optimize "Full"
