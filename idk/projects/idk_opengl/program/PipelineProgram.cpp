#include "pch.h"
#include "PipelineProgram.h"
#include <idk_opengl/program/Program.h>

namespace idk::ogl
{
	PipelineProgram::PipelineProgram()
	{
		glGenProgramPipelines(1, &_pipeline);
	}
	PipelineProgram::~PipelineProgram()
	{
		glDeleteProgramPipelines(1, &_pipeline);
	}

	void PipelineProgram::PushProgram(RscHandle<ShaderProgram> gen_program)
	{
		auto& program = gen_program.as<Program>();

		_programs.erase(std::remove_if(_programs.begin(), _programs.end(), [&program, this](RscHandle<ShaderProgram> prog_handle)->bool
		{
			auto& prog = prog_handle.as<Program>();
			if (prog.ShaderFlags() & program.ShaderFlags())
			{
				glUseProgramStages(_pipeline, prog.ShaderFlags(), 0);
				return true;
			}
			else
				return false;

		}), _programs.end());

		_programs.emplace_back(gen_program);
		glUseProgramStages(_pipeline, program.ShaderFlags(), program.ID());
	}

	void PipelineProgram::PopProgram(GLenum shader_flags)
	{
		_programs.erase(std::remove_if(_programs.begin(), _programs.end(), [shader_flags, this](RscHandle<ShaderProgram> prog_handle)->bool
		{
			auto& prog = prog_handle.as<Program>();
			if (prog.ShaderFlags() & shader_flags)
			{
				glUseProgramStages(_pipeline, prog.ShaderFlags(), 0);
				return true;
			}
			else
				return false;

		}), _programs.end());
	}
	void PipelineProgram::PopAllPrograms()
	{
		_programs.clear();
		glUseProgramStages(_pipeline, GL_ALL_SHADER_BITS, 0);
	}

	void PipelineProgram::Use()
	{
		glBindProgramPipeline(_pipeline);
	}

	PipelineProgram::PipelineProgram(PipelineProgram&& rhs) noexcept
		: _pipeline{rhs._pipeline}, _programs{std::move(rhs._programs)}
	{
		rhs._pipeline = 0;
	}
	PipelineProgram& PipelineProgram::operator=(PipelineProgram&& rhs) noexcept
	{
		std::swap(_pipeline, rhs._pipeline);
		std::swap(_programs, rhs._programs);
		return *this;
	}
}