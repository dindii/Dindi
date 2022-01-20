#pragma once

#define GLR_ASSERT(x) if(x) { } \
else { __debugbreak();}

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

#define BIT(x) (1 << x)

//Quick workaround to avoid copy and paste .exe waste of time. Move this to a copy post build command
//but, to be honest, SandBox will hold the resources in the future, so we won't need anything of this.
//Although, this is useful to engine default stuff like shader and materials etc
#define RESOURCES_PATH "../Boids/"