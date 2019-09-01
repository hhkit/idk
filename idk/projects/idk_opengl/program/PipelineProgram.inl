#pragma once
#include <glad/glad.h>
#include <idk_opengl/program/Program.h>

namespace idk::ogl
{
	template<typename T>
	void PipelineProgram::SetUniform(std::string_view uniform, const T& obj)
	{
		for (auto& elem : _programs)
			elem.as<Program>().SetUniform(uniform, obj);
	}
}