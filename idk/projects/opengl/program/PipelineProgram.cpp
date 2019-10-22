#include "pch.h"
#include "PipelineProgram.h"
#include <opengl/program/Program.h>
#include <opengl/resource/OpenGLTexture.h>
#include <opengl/resource/OpenGLCubemap.h>
#include <iostream>

namespace idk::ogl
{
	void DebugPipeline(GLuint pipeline)
	{
		GLint length = 0;
		glGetProgramPipelineiv(pipeline, GL_INFO_LOG_LENGTH, &length);
		if (length)
		{
			std::string infolog;
			infolog.resize(length + 1);

			glGetProgramPipelineInfoLog(pipeline, length, &length, infolog.data());
			std::cout << infolog << "\n";
		}
	}


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
		if (!gen_program)
			return;
		auto& program = gen_program.as<Program>();
		if (program.ShaderFlags() == 0)
			return;

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
		DebugPipeline(_pipeline);

	}

	void PipelineProgram::PopProgram(GLenum shader_flags)
	{
		_programs.erase(std::remove_if(_programs.begin(), _programs.end(), [shader_flags, this](RscHandle<ShaderProgram> prog_handle)->bool
		{
			auto& prog = prog_handle.as<Program>();
			if (prog.ShaderFlags() & shader_flags)
			{
				glUseProgramStages(_pipeline, prog.ShaderFlags(), 0);
				DebugPipeline(_pipeline);
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
		DebugPipeline(_pipeline);
	}

	void PipelineProgram::Use()
	{
		glBindProgramPipeline(_pipeline);
	}

	bool PipelineProgram::SetUniform(std::string_view uniform, const RscHandle<OpenGLTexture>& texture, GLuint texture_unit)
	{
		texture->BindToUnit(texture_unit);
		return SetUniform(uniform, texture_unit);
	}

	bool PipelineProgram::SetUniform(std::string_view uniform, const RscHandle<OpenGLCubemap>& texture, GLuint texture_unit)
	{
		texture->BindToUnit(texture_unit);
		return SetUniform(uniform, texture_unit);
	}

	PipelineProgram::PipelineProgram(PipelineProgram&& rhs) noexcept
		: _pipeline{ rhs._pipeline }, _programs{ std::move(rhs._programs) }
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