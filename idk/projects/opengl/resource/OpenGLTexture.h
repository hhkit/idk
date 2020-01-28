#pragma once
#include <gfx/Texture.h>
#include <gfx/CompiledTexture.h>
#include <gfx/TextureInternalFormat.h>
#include <glad/glad.h>

namespace idk::ogl
{
	class OpenGLTexture final
		: public Texture
	{
	public:
		OpenGLTexture();
		OpenGLTexture(TextureInternalFormat format, uvec2 size, unsigned mip_level = 0);
		explicit OpenGLTexture(const CompiledTexture&);
		OpenGLTexture(OpenGLTexture&&);
		OpenGLTexture& operator=(OpenGLTexture&&);
		~OpenGLTexture();

		void Bind();
		void BindToUnit(GLuint texture_unit = 0);

		void Buffer(	
			void* data, size_t buffer_size, 
			uvec2 texture_size, 
			TextureInternalFormat format, 
			GLenum incoming_components = GL_RGB, GLenum incoming_type = GL_UNSIGNED_BYTE
		);
		void Buffer(void* data, size_t buffer_size, uvec2 texture_size, ColorFormat format, bool compressed);

		using Texture::Size;
		uvec2 Size(uvec2 new_size) override;
		virtual void* ID() const override;
		
	private:
		GLuint _id = 0;
		bool   _is_compressed = false;
		GLuint _mip_level = 0;

		void SetUVMode(UVMode);
		void SetFilteringMode(FilterMode);
	};
}