#pragma once
#include <glad/glad.h>
#include <gfx/ShaderProgram.h>

namespace idk::ogl
{
	class Program;

	class PipelineProgram
	{
	public:
		PipelineProgram();
		~PipelineProgram();

		void PushProgram(RscHandle<ShaderProgram>);
		void PopProgram(GLenum shader_flags);
		void PopAllPrograms();

		void Use();
		template<typename T>
		void SetUniform(std::string_view uniform, const T& obj);

		PipelineProgram(PipelineProgram&&) noexcept;
		PipelineProgram& operator=(PipelineProgram&&) noexcept;
	private:
		GLuint _pipeline;
		vector<RscHandle<ShaderProgram>> _programs;
	};
}
#include "PipelineProgram.inl"