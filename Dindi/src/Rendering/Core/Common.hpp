#pragma once

#include "Dindipch.h"
//#include <glad/glad.h>
#include "../../../vendor/glad/include/glad/glad.h"
#include <stdint.h>
#include <glm/vec4.hpp>
#include <utils/Logger.h>

namespace Dindi
{
	enum RenderTargetInternalFormat
	{
		DND_NONE = 0,
		DND_DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8,
		DND_DETPH_UNSIZED = GL_DEPTH_COMPONENT,
		DND_DETPH_32 = GL_DEPTH_COMPONENT32,
		DND_COLOR_SRGB8 = GL_SRGB8,
		DND_COLOR_SRGBA8 = GL_SRGB8_ALPHA8,
		DND_COLOR_RGB8  = GL_RGB8,
		DND_R8 = GL_R8,
		DND_COLOR_RGBA32F = GL_RGBA32F,
		DND_COLOR_RGBA16F = GL_RGBA16F,
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

		case DND_DETPH_UNSIZED: case DND_DETPH_32:
			return RenderTargetFormat::DND_DEPTH;
			break;

		case DND_COLOR_SRGB8: case DND_COLOR_RGB8:
			return RenderTargetFormat::DND_RGB;
			break;

		case DND_COLOR_SRGBA8: case DND_COLOR_RGBA16F : case DND_COLOR_RGBA32F:
			return RenderTargetFormat::DND_RGBA;
			break;

		case DND_R8:
			return RenderTargetFormat::DND_RED;
			break;

		default:
				DND_LOG_WARNING("Cannot identify RenderTargetInternalFormat!");
			break;
		}
	}

	enum CullingFaceMode
	{
		FRONT,
		BACK
	};

	enum FramebufferRenderTargetSlot
	{
		POSITION,
		NORMAL,
		ALBEDO_SPECULAR,
		DEPTH,
		FRAME_BUFFER_RENDER_TARGET_SLOT_LAST = DEPTH
	};

	enum RenderTargetMagMinFilter
	{
		NEAREST,
		LINEAR
	};

	enum RenderTargetWrapMode
	{
		REPEAT,
		STRETCH,
		CLAMP_BORDER
	};

	struct RenderTargetDescriptor
	{
		RenderTargetInternalFormat internalFormat = DND_NONE;
		RenderTargetDataType type;
		uint32_t width, height;
		RenderTargetMagMinFilter minFilter;
		RenderTargetMagMinFilter magFilter;
		RenderTargetWrapMode wrapU;
		RenderTargetWrapMode wrapV;
		glm::vec4 borderColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

}