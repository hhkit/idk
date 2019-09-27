#pragma once
#include <idk.h>
#include <res/ResourceHandle.h>
#include <idk_opengl/resource/OpenGLTexture.h>
#include <ds/dual_set.h>
#include <glad/glad.h>

namespace idk::ogl
{
	class TextureManager
	{
	public:
		using TextureUnit = GLint;
		TextureManager();
		TextureUnit Bind(RscHandle<ogl::OpenGLTexture>);
		void UnbindAll();
	private:
		dual_set<TextureUnit, RscHandle<ogl::OpenGLTexture>> texture_units;
		const GLuint max_textures{8};
	};
}