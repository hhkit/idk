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
		void PopAllPrograms();

		void Use();
		template<typename T>
		void SetUniform(std::string_view uniform, const T& obj);

		PipelineProgram(PipelineProgram&&) noexcept;
		PipelineProgram& operator=(PipelineProgram&&) noexcept;
	private:
		GLuint _pipeline;
		vector<const Program*> _programs;
	};
}
#include "PipelineProgram.inl"