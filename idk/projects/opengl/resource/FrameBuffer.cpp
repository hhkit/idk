#include "pch.h"
#include "FrameBuffer.h"
#include <core/Core.h>
#include <opengl/resource/OpenGLTexture.h>
#include <res/ResourceHandle.inl>
#include <res/ResourceMeta.inl>
#include <res/ResourceManager.inl>
#include <res/Guid.inl>

namespace idk::ogl
{
	OpenGLRenderTarget::OpenGLRenderTarget()
	{
		//glGenRenderbuffers(1, &depthbuffer);
		//glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
	}

	//OpenGLRenderTarget::~OpenGLRenderTarget()
	//{
	//	//for (auto& elem : textures)
	//	//	Core::GetResourceManager().Free(elem);
	//	//glDeleteRenderbuffers(1, &depthbuffer);
	//}

	void OpenGLRenderTarget::OnFinalize()
	{
		for (auto& elem : Textures())
			Core::GetResourceManager().Free(elem);

		array<TextureInternalFormat, 2> formats {
			TextureInternalFormat::RGBA_16_F,
			TextureInternalFormat::DEPTH_16
		};

		for (auto [elem, fmt] : zip(Textures(), formats))
		{
			auto tex = elem = (elem == RscHandle<Texture>{})? 
				  Core::GetResourceManager().LoaderEmplaceResource<OpenGLTexture>(fmt, size)
				: Core::GetResourceManager().LoaderEmplaceResource<OpenGLTexture>(elem.guid, fmt, size);
		}

		auto tex = Textures()[kDepthIndex];
		depthbuffer = s_cast<GLuint>(r_cast<intptr_t>(tex->ID()));
	}

	GLuint OpenGLRenderTarget::DepthBuffer() const
	{
		return depthbuffer;
	}
}