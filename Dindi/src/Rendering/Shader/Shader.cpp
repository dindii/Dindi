#include "Dindipch.h"
#include "Shader.h"
#include <glad/glad.h>
#include "Utils/Logger.h"

#include <fstream>
#include <sstream>

#include "Rendering/Shader/ShaderHotReloader.h"

namespace Dindi
{
	Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
		: m_VertexShaderFilepath(vertexSource), m_FragmentShaderFilepath(fragmentSource)
	{
		Init(vertexSource, fragmentSource);
	}
	
	void Shader::Init(const std::string& vertexSource, const std::string& fragmentSource)
	{
		ParseShaderFiles(vertexSource, fragmentSource);
	}

	void Shader::ReloadShader()
	{
		ParseShaderFiles(m_VertexShaderFilepath, m_FragmentShaderFilepath, true);
	}

	bool Shader::ParseShaderFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, bool hotreload)
	{
		std::ifstream vertexShader, fragmentShader;

		vertexShader.open(vertexShaderPath);
		fragmentShader.open(fragmentShaderPath);

		if(vertexShader && fragmentShader)
		{ 		
			std::stringstream vertexShaderStream, fragmentShaderStream;

			vertexShaderStream << vertexShader.rdbuf();
			fragmentShaderStream << fragmentShader.rdbuf();

			vertexShader.close();
			fragmentShader.close();

			CreateShader(vertexShaderStream.str(), fragmentShaderStream.str(), hotreload);
		}
		else
		{
			DND_LOG_ERROR("ERROR READING SHADER FILES!");
			return false;
		}
		
		return true;

	}

	bool Shader::CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, bool hotreload)
	{
	
		uint32_t program = glCreateProgram();
		uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
		uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

		if (!vs || !fs)
		{
			glDeleteShader(vs);
			glDeleteShader(fs);
			glDeleteProgram(program);
			return false;
		}

		//if we are hot reloading and the compilation went fine, we will delete the old shader program so we can 
		//bind the new one and change the id.
		if (hotreload && m_RendererID)
		{
			glDeleteProgram(m_RendererID);
		}

		
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);

		//@TODO: DETTACH
		m_RendererID = program;

		glDeleteShader(vs);
		glDeleteShader(fs);

		if(!hotreload)
			ShaderHotReloader::AddShader(*this);

		return true;
	}

	unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
	{
		unsigned int shaderId = glCreateShader(type);
		const char* src = source.c_str();

		glShaderSource(shaderId, 1, &src, nullptr);
		glCompileShader(shaderId);

		int compileResult;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileResult);

		if (!compileResult)
		{
			int length;
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);

			char* message = new char[length]; 
			glGetShaderInfoLog(shaderId, length, &length, message);

			if (type == GL_FRAGMENT_SHADER)
				DND_LOG_ERROR("Failed to compile Fragment Shader: ", message);

			else if (type == GL_VERTEX_SHADER)
				DND_LOG_ERROR("Failed to compile Vertex Shader: ", message);

			else if (type == GL_GEOMETRY_SHADER)
				DND_LOG_ERROR("Failed to compile Geometry Shader: ", message);

			glDeleteShader(shaderId);
			delete[] message;
			return 0;
		}

		return shaderId;
	}

	Shader::~Shader()
	{
		//Just to test some stuff without RAII interference
		glDeleteProgram(m_RendererID);
	}


	void Shader::Bind()
	{
		glUseProgram(m_RendererID);
	}

	void Shader::UnBind()
	{
		glUseProgram(0);
	}

	void Shader::UploadUniformMat4(const std::string& name, const mat4& mat)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str()); //@TODO: Cache those locations
		glUniformMatrix4fv(location, 1, GL_FALSE, &mat.elements[0]);
		
	}

	void Shader::UploadUniformFloat4(const std::string& name, const vec4& mat)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str()); //@TODO: Cache those locations
		glUniform4f(location, mat.x, mat.y, mat.z, mat.w);
	}

	void Shader::UploadIntArray(const std::string& name, int* data, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str()); //@TODO: Cache those locations
		glUniform1iv(location, count, data);
	}

}