#pragma once
#include <idk.h>
#include <glad/glad.h>
#include <gfx/ShaderProgram.h>

namespace idk::ogl
{
	class Shader
	{
	public:
		Shader() = default;
		Shader(GLenum shader_type, string_view shader_code);
		~Shader();

		Shader(Shader&&) noexcept;
		Shader& operator=(Shader&&) noexcept;
	private:
		GLuint _shader_id = 0;
		GLenum _shader_type = 0;
		GLint  _shader_flags = 0;

		friend class Program;
	};

	class Program
		: public ShaderProgram
	{
	public:
		Program() = default;
		~Program();

		// accessors
		GLint ID() const;
		GLint ShaderFlags() const;

		Program& Attach(Shader&& shader);
		Program& Link();
		template<typename T>
		bool SetUniform(std::string_view uniform, const T& obj);

		Program(Program&&) noexcept;
		Program& operator=(Program&&) noexcept;
	private:
		GLuint _program_id = 0;
		GLint  _shader_flags = 0;

		vector<Shader> _shaders{};
	};
}
#include "Program.inl"