#pragma once
#include <glad/glad.h>

namespace idk::ogl
{
	class Program;

	class PipelineProgram
	{
	public:
		PipelineProgram();
		~PipelineProgram();

		void PushProgram(const Program&);
		void PopProgram(GLenum shader_flags);

		void Use();

		PipelineProgram(PipelineProgram&&) noexcept;
		PipelineProgram& operator=(PipelineProgram&&) noexcept;
	private:
		GLuint _pipeline;
		vector<const Program*> _programs;
	};
}