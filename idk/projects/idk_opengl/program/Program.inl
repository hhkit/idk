#pragma once

namespace idk::ogl
{
	template<typename T>
	bool Program::SetUniform(std::string_view uniform, const T& obj)
	{
		auto loc = glGetUniformLocation(_program_id, uniform.data());
		if (loc < 0) return false;

		if constexpr (std::is_same_v<T, bool>)   glProgramUniform1i(_program_id, loc, obj);
		if constexpr (std::is_same_v<T, int>)    glProgramUniform1i(_program_id, loc, obj);
		if constexpr (std::is_same_v<T, float>)  glProgramUniform1f(_program_id, loc, obj);
		if constexpr (std::is_same_v<T, ivec2>)  glProgramUniform2i(_program_id, loc, obj[0], obj[1]);
		if constexpr (std::is_same_v<T, ivec3>)  glProgramUniform3i(_program_id, loc, obj[0], obj[1], obj[2]);
		if constexpr (std::is_same_v<T, ivec4>)  glProgramUniform4i(_program_id, loc, obj[0], obj[1], obj[2], obj[3]);
		if constexpr (std::is_same_v<T, vec2>)   glProgramUniform2f(_program_id, loc, obj[0], obj[1]);
		if constexpr (std::is_same_v<T, vec3>)   glProgramUniform3f(_program_id, loc, obj[0], obj[1], obj[2]);
		if constexpr (std::is_same_v<T, vec4>)   glProgramUniform4f(_program_id, loc, obj[0], obj[1], obj[2], obj[3]);
		if constexpr (std::is_same_v<T, mat3>)   glProgramUniformMatrix3fv(_program_id, loc, 1, GL_FALSE, obj.data());
		if constexpr (std::is_same_v<T, mat4>)   glProgramUniformMatrix4fv(_program_id, loc, 1, GL_FALSE, obj.data());

		return true;
	}
}