#pragma once
#include <gfx/Texture.h>
#include <glad/glad.h>

namespace idk::ogl
{
	class OpenGLTexture
		: public Texture
	{
	public:
		OpenGLTexture();
		OpenGLTexture(OpenGLTexture&&);
		OpenGLTexture& operator=(OpenGLTexture&&);
		~OpenGLTexture();

		void Bind();
		void BindToUnit(GLuint texture_unit = 0);
		void Buffer(void* data, ivec2 size, ColorFormat format_in);

		virtual void* ID() const;
		
	private:
		GLuint _id = 0;
		void OnMetaUpdate(const TextureMeta&);
		void UpdateUV(UVMode);
	};
}