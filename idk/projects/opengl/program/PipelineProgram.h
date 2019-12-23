#pragma once
#include <glad/glad.h>
#include <gfx/ShaderProgram.h>
namespace idk::ogl
{
	class Program;
	class OpenGLTexture;
	class OpenGLCubemap;

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
		bool SetUniform(std::string_view uniform, const T& obj);
		bool SetUniform(std::string_view uniform, const RscHandle<OpenGLTexture>& texture, GLuint texture_unit);
		bool SetUniform(std::string_view uniform, const RscHandle<OpenGLCubemap>& texture, GLuint texture_unit);

		PipelineProgram(PipelineProgram&&) noexcept;
		PipelineProgram& operator=(PipelineProgram&&) noexcept;
	private:
		GLuint _pipeline;
		vector<RscHandle<ShaderProgram>> _programs;
	};
}
