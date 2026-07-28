#pragma once

#include "Core/Core.h"
#include <Rendering/Core/Common.hpp>
#include <string>

namespace Dindi
{
	enum ETextureLoadFlag : uint8_t
	{
		NONE = 0,
		LOAD_AS_SRGB = BIT(0),
		LOAD_AS_RGB = BIT(1)
	};

	class Texture2D
	{
	public:
		static Ref<Texture2D> Load(const std::string& path, ETextureLoadFlag flags = ETextureLoadFlag::NONE);
		Texture2D(const uint8_t* data, const uint32_t width, const uint32_t height, const uint32_t channels = 4, ETextureLoadFlag flags = ETextureLoadFlag::NONE);
		Texture2D(const RenderTargetDescriptor& rtDescriptor, ETextureLoadFlag flags = ETextureLoadFlag::NONE);

		Texture2D();

		virtual ~Texture2D();

		virtual uint32_t GetWidth()   const { return m_Width; }
		virtual uint32_t GetHeight()  const { return m_Height; }
		virtual uint32_t GetID()      const { return m_RendererID; }
		virtual std::string GetPath()const { return m_Path; }
		virtual uint32_t GetInternalFormat() const { return m_InternalFormat; }
		virtual void SetData(void* data);
		virtual void SetID(uint32_t id) { m_RendererID = id;}

		virtual void Bind(uint32_t slot = 0) const;

		bool operator!() { return m_RendererID == 0 || m_RendererID < 0; }
		bool operator==(const Texture2D& other) const { return (this->m_RendererID == other.GetID()); };
	private:
		Texture2D(const std::string&, ETextureLoadFlag flags = ETextureLoadFlag::NONE);
		void LoadTextureFromData(const uint8_t* data, const uint32_t width, const uint32_t height, const uint32_t channels, ETextureLoadFlag flags = ETextureLoadFlag::NONE);

	private:
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		std::string m_Path;
		uint32_t m_InternalFormat, m_DataFormat;
	};
}