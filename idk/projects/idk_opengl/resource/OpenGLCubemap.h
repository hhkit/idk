#pragma once
#include <gfx/CubeMap.h>
#include <glad/glad.h>

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
		void Buffer(unsigned int face_value, void* data, ivec2 size, CMInputChannels inputchn = CMInputChannels::RGB, CMColorFormat cFormat = CMColorFormat::RGBAF_16);

		using CubeMap::Size;
		void Size(ivec2 new_size) override;
		virtual void* ID() const override;
		span<const GLuint> ConvolutedID() const;
	private:
		GLuint _id = 0;
		GLuint _convoluted_id[2]{};
		void OnMetaUpdate(const CubeMapMeta&) override;
		void UpdateUV(CMUVMode);
	};
}