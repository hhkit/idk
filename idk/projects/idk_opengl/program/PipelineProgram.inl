#pragma once
#include <glad/glad.h>
#include <idk_opengl/program/Program.h>
namespace idk::ogl
{
	template<typename T>
	void PipelineProgram::SetUniform(std::string_view uniform, const T& obj)
	{
		for (auto& program : _programs)
		{
			auto loc = glGetUniformLocation(program->_program_id, uniform.data());

			if constexpr (std::is_same_v<T, bool>)   glProgramUniform1i(program->_program_id, loc, obj);
			if constexpr (std::is_same_v<T, int>)    glProgramUniform1i(program->_program_id, loc, obj);
			if constexpr (std::is_same_v<T, float>)  glProgramUniform1f(program->_program_id, loc, obj);
			if constexpr (std::is_same_v<T, ivec2>)  glProgramUniform2f(program->_program_id, loc, obj[0], obj[1]);
			if constexpr (std::is_same_v<T, ivec3>)  glProgramUniform3f(program->_program_id, loc, obj[0], obj[1], obj[2]);
			if constexpr (std::is_same_v<T, ivec4>)  glProgramUniform4f(program->_program_id, loc, obj[0], obj[1], obj[2], obj[3]);
			if constexpr (std::is_same_v<T, vec2>)   glProgramUniform2f(program->_program_id, loc, obj[0], obj[1]);
			if constexpr (std::is_same_v<T, vec3>)   glProgramUniform3f(program->_program_id, loc, obj[0], obj[1], obj[2]);
			if constexpr (std::is_same_v<T, vec4>)   glProgramUniform4f(program->_program_id, loc, obj[0], obj[1], obj[2], obj[3]);
			if constexpr (std::is_same_v<T, mat3>)   glProgramUniformMatrix3fv(program->_program_id, loc, 1, GL_FALSE, obj.data());
			if constexpr (std::is_same_v<T, mat4>)   glProgramUniformMatrix4fv(program->_program_id, loc, 1, GL_FALSE, obj.data());
		}
	}
}