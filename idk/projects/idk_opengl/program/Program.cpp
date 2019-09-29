#include "pch.h"
#include <core/Core.h>
#include "Program.h"
#include <gfx/IShaderProgramFactory.h>
#include <iostream>

constexpr auto replacer = R"(
#ifdef OGL
#define U_LAYOUT(SET, BIND) 
#define S_LAYOUT(SET, BIND) 
#define BLOCK(X) struct X
#endif
#ifdef VULKAN
#define U_LAYOUT(SET, BIND) layout(set = SET, binding = BIND) 
#define S_LAYOUT(SET, BIND) layout(std140,set = SET, binding = BIND) 
#define BLOCK(X) X
#endif
)";

namespace idk::ogl
{
	Shader::Shader(GLenum shader_type, string_view shader_code)
	{
		_shader_id = glCreateShader(shader_type);

		auto version_pos = shader_code.find("#version");
		auto version_end = shader_code.find("\n", version_pos);

		const char* arr[] = { shader_code.substr(version_pos).data(), "#define OGL\n", replacer, shader_code.substr(version_end).data() } ;
		const GLint lengths[] = { (GLint)(version_end - version_pos + 1), -1, -1, -1};

		glShaderSource(_shader_id, sizeof(arr) / sizeof(*arr), arr, lengths);
		glCompileShader(_shader_id);
		
		{
			int success = 0;
			char infoLog[512];
			glGetShaderiv(_shader_id, GL_COMPILE_STATUS, &success);

			constexpr auto get_shader = [](GLenum shader_t) ->std::string_view
			{
				switch (shader_t)
				{
				case GL_VERTEX_SHADER:          return "Vertex Shader";
				case GL_TESS_CONTROL_SHADER:    return "Tesselation Control Shader";
				case GL_TESS_EVALUATION_SHADER: return "Tesselation Evaluation Shader";
				case GL_GEOMETRY_SHADER:        return "Geometry Shader";
				case GL_FRAGMENT_SHADER:        return "Fragment Shader";
				case GL_COMPUTE_SHADER:         return "Compute Shader";
				default:                        return "Non-Existent Shader";
				}
			};

			if (!success)
			{
				glGetShaderInfoLog(_shader_id, 512, NULL, infoLog);

				std::cout << get_shader(shader_type) << " compilation failed!\n";
				std::cout << infoLog << "\n";
			}

			else
			{
				std::cout << get_shader(shader_type) << " compilation succeeded!\n";
			}
		}
		_shader_flags |= [](GLenum shader_t)
		{
			switch (shader_t)
			{
			case GL_VERTEX_SHADER:          return GL_VERTEX_SHADER_BIT;
			case GL_TESS_CONTROL_SHADER:    return GL_TESS_CONTROL_SHADER_BIT;
			case GL_TESS_EVALUATION_SHADER: return GL_TESS_EVALUATION_SHADER_BIT;
			case GL_GEOMETRY_SHADER:        return GL_GEOMETRY_SHADER_BIT;
			case GL_FRAGMENT_SHADER:        return GL_FRAGMENT_SHADER_BIT;
			case GL_COMPUTE_SHADER:         return GL_COMPUTE_SHADER_BIT;
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

	GLint Program::ID() const
	{
		return _program_id;
	}

	GLint Program::ShaderFlags() const
	{
		return _shader_flags;
	}


	Program& Program::Attach(Shader&& shader)
	{
		_shader_flags |= shader._shader_flags;
		_shaders.emplace_back(std::move(shader));
		return *this;
	}

	Program& Program::Link()
	{
		if (_program_id)
			glDeleteProgram(_program_id);

		_program_id = glCreateProgram();
		glProgramParameteri(_program_id, GL_PROGRAM_SEPARABLE, GL_TRUE);
		for (auto& elem : _shaders)
			glAttachShader(_program_id, elem._shader_id);
		glLinkProgram(_program_id);
		{
			GLint success = 0;
			char infoLog[512];
			glGetProgramiv(_program_id, GL_LINK_STATUS, &success);

			if (!success)
			{
				glGetProgramInfoLog(_program_id, 512, NULL, infoLog);

				std::cout << "Program link failed!\n";
				std::cout << infoLog << "\n";
			}
			else
			{
				std::cout << "Program link succeeded!\n";

				GLint uniform_count;
				glGetProgramiv(_program_id, GL_ACTIVE_UNIFORMS, &uniform_count);
				char buf[256]{};
				for (GLint i = 0; i < uniform_count; ++i)
				{
					glGetActiveUniformName(_program_id, i, 256, nullptr, buf);
					std::cout << "  uniform detected: " << buf << "\n";
					std::cout << "  location: " << glGetUniformLocation(_program_id, buf) << "\n";
					//std::cout << "  location: " << glGetUniformBlockIndex(_program_id, buf) << "\n";
				}

				GLint numBlocks = 0;
				glGetProgramInterfaceiv(_program_id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
				std::cout << "  blocks found: " << numBlocks << "\n";

				for (GLint i = 0; i < numBlocks; ++i)
				{
					GLint val = 0;
					glGetActiveUniformBlockiv(_program_id, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &val);
					std::cout << " uniforms in block " << i << ": " << val << "\n";
					for (GLint j = 0; j < val; ++j)
					{

					}
				}
			}
		}

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