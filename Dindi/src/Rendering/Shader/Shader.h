#pragma once
#include "Math/mat4.h"
#include <string>

//#NOTE The use of std::string here is kinda of a bad idea with the hot reloader because we don't want to
// allocate memory every frame just for some checks. Profile this when we got some bottleneck.

namespace Dindi
{
	class Shader
	{
	public:
		Shader(const std::string& vertexSource, const std::string& fragmentSource);
		Shader(const Shader& other) { *this = other; }
		Shader() = default;
		virtual ~Shader();

		virtual void Init(const std::string& vertexSource, const std::string& fragmentSource);

		void ReloadShader();

		std::string GetVertexShaderFilePath()   const { return m_VertexShaderFilepath; }
		std::string GetFragmentShaderFilePath() const { return m_FragmentShaderFilepath; }

		virtual void Bind();
		virtual void UnBind();
		virtual void UploadUniformMat4(const std::string& name, const mat4& mat);
		virtual void UploadUniformFloat4(const std::string& name, const vec4& mat);
		virtual void UploadIntArray(const std::string& name, int* data, uint32_t count);
		uint32_t GetID() const { return m_RendererID; }

		void operator=(const Shader& other) { m_RendererID = other.GetID(); }
	private:
		virtual bool ParseShaderFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, bool hotreload = false);
		virtual bool CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, bool hotreload = false);
		virtual unsigned int CompileShader(unsigned int type, const std::string& source);

	private:
		uint32_t m_RendererID;
		std::string m_VertexShaderFilepath, m_FragmentShaderFilepath;
	};
}