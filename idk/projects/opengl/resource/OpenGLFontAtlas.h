#pragma once
#include <gfx/FontAtlas.h>
#include <glad/glad.h>
#include <opengl/resource/OpenGLTexture.h>

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
		void Buffer(void* data, ivec2 size, InputChannels format_in = InputChannels::RGB, ColorFormat internalFormat_in = ColorFormat::SRGB, const unsigned& mipmap_size = 0, const float& imgSize = 0.f);

		using FontAtlas::Size;
		void Size(ivec2 new_size) override;
		virtual void* ID() const override;

		RscHandle<Texture> Tex()const noexcept;

		RscHandle<OpenGLTexture> texture;

	private:
		GLuint _id = 0;
		bool _isCompressedTexture{ false };
		void OnMetaUpdate(const FontAtlasMeta&);
		void UpdateUV(UVMode);
		void UpdateFilter(FilterMode, const bool& isMipMap = false);
	};
}