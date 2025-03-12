#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <Core/Core.h>
#include <Rendering/Texture/Texture2D.h>

//#NOTE The use of std::string here is kinda of a bad idea with the hot reloader because we don't want to
// allocate memory every frame just for some checks. Profile this when we got some bottleneck.

namespace Dindi
{
	class Shader
	{
	public:
		static Ref<Shader> Load(const std::string& vertexSource, const std::string& fragmentSource);
		Shader(const Shader& other) { *this = other; }
		Shader() = default;
		virtual ~Shader();

		virtual void Init(const std::string& vertexSource, const std::string& fragmentSource);

		void ReloadShader();

		std::string GetVertexShaderFilePath()   const { return m_VertexShaderFilepath; }
		std::string GetFragmentShaderFilePath() const { return m_FragmentShaderFilepath; }

		virtual void Bind();
		virtual void UnBind();
		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& mat);
		virtual void UploadUniformFloat(const std::string& name, const float value);
		virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& mat);
		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& mat);
		virtual void UploadIntArray(const std::string& name, int* data, uint32_t count);
		virtual void UploadInt(const std::string& name, uint32_t data);
		uint32_t GetID() const { return m_RendererID; }

		void operator=(const Shader& other) { m_RendererID = other.GetID(); }
	private:
		Shader(const std::string& vertexSource, const std::string& fragmentSource);
		
		virtual bool ParseShaderFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, bool hotreload = false);
		virtual bool CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, bool hotreload = false);
		virtual unsigned int CompileShader(unsigned int type, const std::string& source);

	private:
		uint32_t m_RendererID;
		std::string m_VertexShaderFilepath, m_FragmentShaderFilepath;
	};
}