#include "pch.h"
#include "FrameBuffer.h"
#include <core/Core.h>
#include <gfx/Texture.h>

namespace idk::ogl
{
	FrameBuffer::FrameBuffer()
	{
		glGenRenderbuffers(1, &depthbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, meta.size.x, meta.size.y);
		meta.textures.emplace_back();
	}

	FrameBuffer::~FrameBuffer()
	{
		for (auto& elem : meta.textures)
			Core::GetResourceManager().Free(elem);
		glDeleteRenderbuffers(1, &depthbuffer);
	}

	void FrameBuffer::OnMetaUpdate(const Metadata& newmeta)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, newmeta.size.x, newmeta.size.y);

		for (auto& elem : newmeta.textures)
			elem->Size(newmeta.size);
	}

	GLuint FrameBuffer::DepthBuffer() const
	{
		return depthbuffer;
	}

}