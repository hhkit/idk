#pragma once
#include <gfx/FontAtlas.h>
#include <glad/glad.h>

namespace idk::ogl
{
	class OpenGLFontAtlas
		: public FontAtlas
	{
	public:
		OpenGLFontAtlas();
		OpenGLFontAtlas(const bool& compressed);
		OpenGLFontAtlas(OpenGLFontAtlas&&);
		OpenGLFontAtlas& operator=(OpenGLFontAtlas&&);
		~OpenGLFontAtlas();

		void Bind();
		void BindToUnit(GLuint texture_unit = 0);
		void Buffer(void* data, ivec2 size, FontInputChannels format_in = FontInputChannels::RGB, FontColorFormat internalFormat_in = FontColorFormat::SRGB, const unsigned& mipmap_size = 0, const float& imgSize = 0.f);

		using FontAtlas::Size;
		void Size(ivec2 new_size) override;
		virtual void* ID() const override;

	private:
		GLuint _id = 0;
		bool _isCompressedTexture{ false };
		void OnMetaUpdate(const FontAtlasMeta&);
		void UpdateUV(FontUVMode);
		void UpdateFilter(FontFilterMode, const bool& isMipMap = false);
	};
}