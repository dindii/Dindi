#pragma once

#include <memory>

//#NOTE: I don't think this will be a bottleneck but we should be aware. Our program is not so event driven, so we are ok.
//but once this become a bottleneck in the profiler, change this system for a simple pack event system, like the MS one.
//#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }


#define BIT(x) (1 << x)

 //#TODO: Fix this for shipping.
//Quick workaround to avoid copy and paste .exe waste of time. Move this to a copy post build command
//but, to be honest, SandBox will hold the resources in the future, so we won't need anything of this.
//Although, this is useful to engine default stuff like shader and materials etc
#define RESOURCES_PATH "..\\Dindi\\"

#define DEFAULT_VERTEX_SHADER RESOURCES_PATH "Resources\\Shaders\\DefaultShaderVert.shader"
#define DEFAULT_FRAGMENT_SHADER RESOURCES_PATH "Resources\\Shaders\\DefaultShaderFrag.shader"

//Rendering stuff

enum ERenderingMapSlot
{
	None = 0,
	Diffuse = 1,
	Specular = 2,
	Normal = 3
};

enum EApplicationState
{
	EDITOR = 0, //We are in the editor, we have debug shapes, UI and everything.
	SIMULATION, //We are simulating the game. We don't have any debug shapes but we have UI.
	PLAYING     //Playing, no editor infos.
};

//#NOTE: Please, when changing this, also change in the shaders.
#define DND_MAX_LIGHTS 1000

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args) ...);
}