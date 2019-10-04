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
		OpenGLTexture(const bool& compressed);
		OpenGLTexture(OpenGLTexture&&);
		OpenGLTexture& operator=(OpenGLTexture&&);
		~OpenGLTexture();

		void Bind();
		void BindToUnit(GLuint texture_unit = 0);
		void Buffer(void* data, ivec2 size, InputChannels format_in = InputChannels::RGB, ColorFormat internalFormat_in = ColorFormat::RGBF_32, const unsigned& mipmap_size =0, const float& imgSize=0.f);

		using Texture::Size;
		void Size(ivec2 new_size) override;
		virtual void* ID() const override;
		
	private:
		GLuint _id = 0;
		bool _isCompressedTexture{false};
		void OnMetaUpdate(const TextureMeta&);
		void UpdateUV(UVMode);
		void UpdateFilter(FilterMode, const bool& isMipMap = false);
	};
}