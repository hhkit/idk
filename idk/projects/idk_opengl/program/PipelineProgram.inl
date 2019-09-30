#pragma once
#include <glad/glad.h>
#include <idk_opengl/program/Program.h>

namespace idk::ogl
{
	template<typename T>
	bool PipelineProgram::SetUniform(std::string_view uniform, const T& obj)
	{
		bool set = false;
		for (auto& elem : _programs)
			set |= elem.as<Program>().SetUniform(uniform, obj);
		return set;
	}
}