project "Glad"
    kind "StaticLib"
    language "C"
	staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "include/glad/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.c"
    }

	includedirs
	{
		"include"
	}
    
    filter "system:windows"
        systemversion "latest"
 

     filter "configurations:Debug"
	 	defines "DINDI_DEBUG"
        runtime "Debug"
        symbols "on"             -- Deixamos on os debug symbols
        
     filter "configurations:Release"
	 	defines "DINDI_RELEASE"
        runtime "Release"
        optimize "on"            -- Ripamos todas as configurações de debug e otimizamos o projeto


     filter "configurations:Dist"
	 defines "DINDI_DIST"
	 runtime "Release"
	 symbols "Off"
	 optimize "Full"