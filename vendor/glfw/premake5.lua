project "GLFW"
    kind "StaticLib"
    language "C"
	staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "include/GLFW/glfw3.h",
		"include/GLFW/glfw3native.h",
    }

	includedirs
	{
		"include"
	}
    
		links
	{
		"lib-vc2017/glfw3.lib"
	}
    filter "system:windows"
        systemversion "latest"
 

     filter "configurations:Debug"
	 	defines "BOIDS_DEBUG"
        runtime "Debug"
        symbols "on"             -- Deixamos on os debug symbols
        
     filter "configurations:Release"
	 	defines "BOIDS_RELEASE"
        runtime "Release"
        optimize "on"            -- Ripamos todas as configurações de debug e otimizamos o projeto


     filter "configurations:Dist"
	 defines "BOIDS_DIST"
	 runtime "Release"
	 symbols "Off"
	 optimize "Full"