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

	void PipelineProgram::PushProgram(const Program& program)
	{
		_programs.erase(std::remove_if(_programs.begin(), _programs.end(), [&program, this](const Program* prog)->bool
		{
			if (prog->_shader_flags & program._shader_flags)
			{
				glUseProgramStages(_pipeline, prog->_shader_flags, 0);
				return true;
			}
			else
				return false;

		}), _programs.end());

		_programs.emplace_back(&program);
		glUseProgramStages(_pipeline, program._shader_flags, program._program_id);
	}

	void PipelineProgram::PopProgram(GLenum shader_flags)
	{
		_programs.erase(std::remove_if(_programs.begin(), _programs.end(), [shader_flags, this](const Program* prog)->bool
		{
			if (prog->_shader_flags & shader_flags)
			{
				glUseProgramStages(_pipeline, prog->_shader_flags, 0);
				return true;
			}
			else
				return false;

		}), _programs.end());
	}
	void PipelineProgram::PopAllPrograms()
	{
		_programs.clear();
		glUseProgramStages(_pipeline, GL_VERTEX_SHADER_BIT | GL_TESS_CONTROL_SHADER_BIT | GL_TESS_EVALUATION_SHADER_BIT | GL_GEOMETRY_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, 0);
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