#pragma once
#include <glad/glad.h>
#include <Utils/Logger.h>
#include <stdint.h>

namespace Dindi
{
	enum RenderTargetInternalFormat
	{
		DND_NONE = 0,
		DND_DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8,
		DND_DETPH_UNSIZED = GL_DEPTH_COMPONENT,
		DND_COLOR_SRGB8 = GL_SRGB8,
		DND_COLOR_SRGBA8 = GL_SRGB8_ALPHA8,
		DND_R8 = GL_R8,
		DND_RGB8 = GL_SRGB8,
	};

	enum RenderTargetFormat
	{
		DND_RGB = GL_RGB,
		DND_RGBA = GL_RGBA,
		DND_DEPTH_STENCIL = GL_DEPTH_STENCIL,
		DND_DEPTH = GL_DEPTH_COMPONENT,
		DND_RED = GL_RED,
	};


	enum RenderTargetDataType
	{
		DND_UNSIGNED_INT_24_8 = GL_UNSIGNED_INT_24_8,
		DND_FLOAT = GL_FLOAT,
		DND_UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
	};

	inline RenderTargetFormat GetFormatFromInternalDataType(RenderTargetInternalFormat internalFormat)
	{
		switch (internalFormat)
		{
			case DND_DEPTH24_STENCIL8:
				return RenderTargetFormat::DND_DEPTH_STENCIL;
				break;

			case DND_DETPH_UNSIZED:
				return RenderTargetFormat::DND_DEPTH;
				break;
			
			case DND_COLOR_SRGB8:
				return RenderTargetFormat::DND_RGB;
				break;
			
			case DND_COLOR_SRGBA8:
				return RenderTargetFormat::DND_RGBA;
				break;
	
			case DND_R8:
				return RenderTargetFormat::DND_RED;
				break;

			default:
				DND_LOG_WARNING("Cannot identify RenderTargetInternalFormat!");
		}
	}


	struct RenderTargetDescriptor
	{
		RenderTargetInternalFormat internalFormat = DND_NONE;
		RenderTargetDataType type;
	};

}