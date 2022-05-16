#pragma once

#define DND_ASSERT(x) if(!x) { } \
else { __debugbreak();}

//#NOTE: I don't think this will be a bottleneck but we should be aware. Our program is not so event driven, so we are ok.
//but once this become a bottleneck in the profiler, change this system for a simple pack event system, like the MS one.
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

#define BIT(x) (1 << x)

 //#TODO: Fix this for shipping.
//Quick workaround to avoid copy and paste .exe waste of time. Move this to a copy post build command
//but, to be honest, SandBox will hold the resources in the future, so we won't need anything of this.
//Although, this is useful to engine default stuff like shader and materials etc
#define RESOURCES_PATH "../Dindi/"

#define DEFAULT_VERTEX_SHADER RESOURCES_PATH "Resources/Shaders/DefaultShaderVert.shader"
#define DEFAULT_FRAGMENT_SHADER RESOURCES_PATH "Resources/Shaders/DefaultShaderFrag.shader"

//Rendering stuff

enum RenderingMapSlot
{
	None = 0,
	Diffuse = 1,
	Specular = 2,
	Normal = 3
};

#define DND_MAX_LIGHTS 1000000