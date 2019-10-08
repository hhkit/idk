#include "pch.h"
#include "FrameBuffer.h"
#include <core/Core.h>
#include <idk_opengl/resource/OpenGLTexture.h>

namespace idk::ogl
{
	FrameBuffer::FrameBuffer()
	{
		glGenRenderbuffers(1, &depthbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, meta.size.x, meta.size.y);
	}

	FrameBuffer::~FrameBuffer()
	{
		for (auto& elem : meta.textures)
			Core::GetResourceManager().Free(elem);
		glDeleteRenderbuffers(1, &depthbuffer);
	}

	void FrameBuffer::OnMetaUpdate(const Metadata& newmeta)
	{
		for (auto& elem : meta.textures)
			Core::GetResourceManager().Free(elem);


		for (auto& elem : newmeta.textures)
		{
			auto tex = Core::GetResourceManager().LoaderEmplaceResource<OpenGLTexture>(elem.guid);
			tex->Size(newmeta.size);
		}
		auto tex = newmeta.textures[kDepthIndex];
		auto tmeta = tex->GetMeta();
		tmeta.internal_format = ColorFormat::DEPTH_COMPONENT;
		tex->SetMeta(tmeta);
		glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, newmeta.size.x, newmeta.size.y);
	}

	GLuint FrameBuffer::DepthBuffer() const
	{
		return depthbuffer;
	}

}