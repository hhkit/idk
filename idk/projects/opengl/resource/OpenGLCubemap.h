#pragma once
#include <gfx/CubeMap.h>
#include <glad/glad.h>

#include <opengl/resource/OpenGLTexture.h>

namespace idk::ogl
{
	class OpenGLCubemap
		: public CubeMap
	{
	public:
		OpenGLCubemap();
		OpenGLCubemap(OpenGLCubemap&&);
		OpenGLCubemap& operator=(OpenGLCubemap&&);
		~OpenGLCubemap();

		void Bind();
		void BindToUnit(GLuint texture_unit = 0);
		void BindConvolutedToUnit(GLuint texture_unit = 0);
		void Buffer(TextureTarget face_value, void* data, ivec2 size, TextureInternalFormat internal_format, GLenum color_components = GL_RGB, GLenum incoming_type = GL_UNSIGNED_BYTE);

		using CubeMap::Size;
		void Size(ivec2 new_size) override;
		virtual void* ID() const override;
		span<const GLuint> ConvolutedID() const;

		RscHandle<Texture> Tex()const noexcept;

		RscHandle<OpenGLTexture> texture;
		RscHandle<OpenGLTexture> convoluted_texture[2];
	private:
		GLuint _id = 0;
		GLuint _convoluted_id[2]{};
		void UpdateUV(UVMode);
	};
}