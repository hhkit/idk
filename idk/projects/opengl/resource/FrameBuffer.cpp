#include "pch.h"
#include "FrameBuffer.h"
#include <core/Core.h>
#include <opengl/resource/OpenGLTexture.h>
#include <res/ResourceHandle.inl>
#include <res/ResourceManager.h>
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


		for (auto& elem : Textures())
		{
			auto tex = elem = (elem == RscHandle<Texture>{})? Core::GetResourceManager().Create<OpenGLTexture>():Core::GetResourceManager().LoaderEmplaceResource<OpenGLTexture>(elem.guid);
			tex->Size(size);
		}
		auto tex = Textures()[kDepthIndex];
		auto tmeta = tex->GetMeta();
		tmeta.internal_format = ColorFormat::DEPTH_COMPONENT;
		tex->SetMeta(tmeta);
		depthbuffer = s_cast<GLuint>(r_cast<intptr_t>(tex->ID()));
		//glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
	}

	GLuint OpenGLRenderTarget::DepthBuffer() const
	{
		return depthbuffer;
	}
}