#pragma once
#include <glad/glad.h>

#include <gfx/FontAtlas.h>
#include <gfx/TextureInternalFormat.h>
#include <opengl/resource/OpenGLTexture.h>

namespace idk::ogl
{
	class OpenGLFontAtlas
		: public FontAtlas
	{
	public:

		OpenGLFontAtlas();
		OpenGLFontAtlas(OpenGLFontAtlas&&);
		OpenGLFontAtlas& operator=(OpenGLFontAtlas&&);
		~OpenGLFontAtlas();

		void Bind();
		void BindToUnit(GLuint texture_unit = 0);
		void Buffer(
			void* data, size_t buffer_size,
			uivec2 texture_size,
			TextureInternalFormat format,
			GLenum incoming_components = GL_RGBA, GLenum incoming_type = GL_UNSIGNED_BYTE
		);

		using FontAtlas::Size;
		void Size(uivec2 new_size) override;
		virtual void* ID() const override;

		RscHandle<Texture> Tex() const noexcept;
	private:
		RscHandle<OpenGLTexture> texture;
	};
}