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
		void Buffer(unsigned int face_value, void* data, ivec2 size, InputChannels inputchn = InputChannels::RGB, ColorFormat cFormat = ColorFormat::RGBAF_16);

		using CubeMap::Size;
		void Size(ivec2 new_size) override;
		virtual void* ID() const override;
		span<const GLuint> ConvolutedID() const;

		RscHandle<Texture> Tex()const noexcept;
		/*span<const RscHandle<Texture>> ConvolutedTex() const;*/

		RscHandle<OpenGLTexture> texture;
		RscHandle<OpenGLTexture> convoluted_texture[2];
	private:
		GLuint _id = 0;
		GLuint _convoluted_id[2]{};
		void OnMetaUpdate(const CubeMapMeta&) override;
		void UpdateUV(UVMode);
	};
}