#include "Dindipch.h"
#include <Rendering/Core/Common.hpp>
#include "Texture2D.h"
#include "Core/Core.h"
#include <stbi/stb_image.h>
#include "Utils/Logger.h"
#include "Utils/AssetManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stbi/stb_image.h"

namespace Dindi
{

	Ref<Texture2D> Texture2D::Load(const std::string& path)
	{
		Ref<Texture2D> texture = AssetManager::Get<Texture2D>(path);

		if (!texture)
		{
			Texture2D* newTex = new Texture2D(path);
			texture.reset(newTex);
			AssetManager::Add<Texture2D>(path, texture);
		}

		return texture;
	}

	Texture2D::Texture2D() : m_Width(0), m_Height(0), m_InternalFormat(0), m_DataFormat(0), m_RendererID(0)
	{

	}

	Texture2D::Texture2D(const RenderTargetDescriptor& rtDescriptor)
	{
		m_Width = rtDescriptor.width;
		m_Height = rtDescriptor.height;
		m_InternalFormat = rtDescriptor.internalFormat;

		RenderTargetFormat format = GetFormatFromInternalDataType(rtDescriptor.internalFormat);
		
		m_DataFormat = format;

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);



		glTexImage2D(GL_TEXTURE_2D, 0, rtDescriptor.internalFormat, rtDescriptor.width, rtDescriptor.height, 0, format, rtDescriptor.type, NULL);


		if(rtDescriptor.minFilter == RenderTargetMagMinFilter::LINEAR)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		else if(rtDescriptor.minFilter == RenderTargetMagMinFilter::NEAREST)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		if (rtDescriptor.magFilter == RenderTargetMagMinFilter::LINEAR)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		else if (rtDescriptor.magFilter == RenderTargetMagMinFilter::NEAREST)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			
		//#TODO: SET THIS INDIVIDUALLY FOR U AND V 
		switch (rtDescriptor.wrapU)
		{
			case RenderTargetWrapMode::STRETCH:
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			} break;
			case RenderTargetWrapMode::CLAMP_BORDER:
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			} break;
			case RenderTargetWrapMode::REPEAT:
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			} break;

			default:
			{
				DND_LOG_WARNING("No wrapping method defined for texture!");
				
			} break;
		}
		
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &rtDescriptor.borderColor[0]);
	}

	//#TODO: We have to split this function. We may want to upload data to the GPU later and not necessarily when it is created.
	void Texture2D::LoadTextureFromData(const uint8_t* data, const uint32_t width, const uint32_t height, const uint32_t channels)
	{
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;

		stbi_set_flip_vertically_on_load(true);

		switch (channels)
		{
		case 1:
		{
			internalFormat = GL_R8;
			dataFormat = GL_RED;
			break;
		}
		case 3:
		{
			internalFormat = GL_SRGB8;
			dataFormat = GL_RGB;
			break;
		}
		case 4:
		{
			internalFormat = GL_SRGB8_ALPHA8;
			dataFormat = GL_RGBA;
			break;
		}
		default:
		{
			DND_LOG_ERROR("Unknown texture format type!");
			return;
		}
		}

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		if (channels == 1)
		{
			// Parameters to avoid ImGui messing up grayscale textures.	
			// This will make sure that the ImGui's shader can output
			// Color(val, val, val, 1.0) instead of Color(1.0, 0.0, 0.0, 1.0).
			glTextureParameteri(m_RendererID, GL_TEXTURE_SWIZZLE_R, GL_RED);
			glTextureParameteri(m_RendererID, GL_TEXTURE_SWIZZLE_G, GL_RED);
			glTextureParameteri(m_RendererID, GL_TEXTURE_SWIZZLE_B, GL_RED);
			glTextureParameteri(m_RendererID, GL_TEXTURE_SWIZZLE_A, GL_ONE);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//This will give support to every dimension, even those that aren't even.
		if (!((width % 2) == 0))
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


		//This will give the user the freedom to start a blank texture so he can fill it later on.
		//The only requisite is to pre-config the texture with width, height and channels to make sure it's valid
		//If stbi could not load the image, we will catch this error in the switch above since we won't have any channel set.
		if (data)
		{
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
			glBindTexture(GL_TEXTURE_2D, 0);
			return;
		}

		DND_LOG_WARNING("Texture's data is not set!");
	}

	Texture2D::Texture2D(const uint8_t* data, const uint32_t width, const uint32_t height, const uint32_t channels /*= 4*/)
	{
		LoadTextureFromData(data, width, height, channels);
	}

	Texture2D::Texture2D(const std::string& path) : m_Path(path)
	{
		int width = 0, height = 0, channels = 0;

		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		if (!data)
		{
			DND_LOG_ERROR("Could not load texture, please check the path!");
			return;
		}

		LoadTextureFromData(data, width, height, channels);
		stbi_image_free(data);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture2D::SetData(void* data)
	{
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void Texture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}