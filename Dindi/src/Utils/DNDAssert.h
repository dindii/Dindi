#pragma once
#include <iostream>

//We will not worry about performance since it is just an assert and it is not meant to be called every frame or so. With this in mind, let's have some flexibility.
#define DND_ASSERT(Expr, Msg) \
    __M_Assert(Expr, __FILE__, __LINE__, Msg)

inline void __M_Assert(bool expr, const char* file, int line, const char* msg)
{
	if (!expr)
	{
		//I will use std::cout with std::endl here because I want the assert message to be immediate. 
		std::cout << "Assert failed:\t" << msg << "\n" << "Source:\t\t" << file << ", line " << line << "\n" << std::endl;
		__debugbreak();
	}
}