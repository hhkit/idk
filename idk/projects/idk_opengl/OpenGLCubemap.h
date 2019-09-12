#pragma once
#include <gfx/Texture.h>
#include <glad/glad.h>

namespace idk::ogl
{
	class OpenGLCubemap
		: public Texture
	{
	public:
		OpenGLCubemap();
		OpenGLCubemap(OpenGLCubemap&&);
		OpenGLCubemap& operator=(OpenGLCubemap&&);
		~OpenGLCubemap();

		void Bind();
		void BindToUnit(GLuint texture_unit = 0);
		void Buffer(unsigned int face_value, void* data, ivec2 size, ColorFormat format_in);

		void Size(ivec2 new_size) override;
		virtual void* ID() const;

	private:
		GLuint _id = 0;
		void OnMetaUpdate(const TextureMeta&);
		void UpdateUV(UVMode);
	};
}