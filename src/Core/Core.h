#pragma once

#define GLR_ASSERT(x) if(x) { } \
else { __debugbreak();}

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

#define BIT(x) (1 << x)