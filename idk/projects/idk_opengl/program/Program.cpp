#include "pch.h"
#include "Program.h"

namespace idk::ogl
{
	Shader::Shader(GLenum shader_type, string_view shader_code)
	{
		_shader_id = glCreateShader(shader_type);
		const char* arr[] = { shader_code.data() } ;
		glShaderSource(_shader_id, 1, arr, 0);
		glCompileShader(_shader_id);

		_shader_flags |= [](GLenum shader_t)
		{
			switch (shader_t)
			{
			case GL_VERTEX_SHADER:          return GL_VERTEX_SHADER_BIT;
			case GL_TESS_CONTROL_SHADER:    return GL_TESS_CONTROL_SHADER_BIT;
			case GL_TESS_EVALUATION_SHADER: return GL_TESS_EVALUATION_SHADER_BIT;
			case GL_GEOMETRY_SHADER:        return GL_GEOMETRY_SHADER_BIT;
			case GL_FRAGMENT_SHADER:        return GL_FRAGMENT_SHADER_BIT;
			default: return 0;
			};
		}(shader_type);
	}

	Shader::~Shader()
	{
		glDeleteShader(_shader_id);
	}

	Shader::Shader(Shader&& rhs) noexcept
		: _shader_id{rhs._shader_id}, _shader_type{rhs._shader_type}, _shader_flags{rhs._shader_flags}
	{
		rhs._shader_id = 0;
		rhs._shader_type = 0;
		rhs._shader_flags = 0;
	}

	Shader& Shader::operator=(Shader&& rhs) noexcept
	{
		std::swap(_shader_id, rhs._shader_id);
		std::swap(_shader_flags, rhs._shader_flags);
		std::swap(_shader_type, rhs._shader_type);
		return *this;
	}

	Program::~Program()
	{
		glDeleteProgram(_program_id);
	}

	Program& Program::Attach(Shader&& shader)
	{
		_shader_flags |= shader._shader_flags;
		_shaders.emplace_back(std::move(shader));
		return *this;
	}

	Program& Program::Link()
	{
		_program_id = glCreateProgram();
		glProgramParameteri(_program_id, GL_PROGRAM_SEPARABLE, GL_TRUE);

		glLinkProgram(_program_id);
		for (auto& elem : _shaders)
			glDetachShader(_program_id, elem._shader_id);
		_shaders.clear();

		return *this;
	}

	Program::Program(Program&& rhs) noexcept
		: _program_id{rhs._program_id}, _shader_flags{rhs._shader_flags}, _shaders{rhs._shader_flags}
	{
		rhs._program_id = 0;
		rhs._shader_flags = 0;
	}

	Program& Program::operator=(Program&& rhs) noexcept
	{
		std::swap(_program_id, rhs._program_id);
		std::swap(_shader_flags, rhs._shader_flags);
		std::swap(_shaders, rhs._shaders);

		return *this;
	}


}