#include "pch.h"
#include "FrameBufferManager.h"
#include <gfx/RenderTarget.h>
#include <idk_opengl/resource/FrameBuffer.h>
#include <idk_opengl/resource/OpenGLTexture.h>

namespace idk::ogl
{
	FrameBufferManager::FrameBufferManager()
	{
		glGenFramebuffers(1, &_fbo_id);
	}

	FrameBufferManager::FrameBufferManager(FrameBufferManager&& rhs)
		: _fbo_id{ rhs._fbo_id }
	{
		rhs._fbo_id = 0;
	}
	FrameBufferManager& FrameBufferManager::operator=(FrameBufferManager&& rhs)
	{
		std::swap(_fbo_id, rhs._fbo_id);
		return *this;
	}
	FrameBufferManager::~FrameBufferManager()
	{
		glDeleteFramebuffers(1, &_fbo_id);
	}
	void FrameBufferManager::SetRenderTarget(RscHandle<FrameBuffer> target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo_id);

		auto& meta = target->GetMeta();
		glViewport(0, 0, meta.size.x, meta.size.y);

		// set texture targets
		vector<GLenum> buffers;

		for (int i = 0; i < meta.textures.size(); ++i)
		{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, (GLuint)meta.textures[i]->ID(), 0);
			buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, (GLuint)target->DepthBuffer(), 0);
		buffers.push_back(GL_DEPTH_ATTACHMENT);

		glDrawBuffers(s_cast<GLsizei>(buffers.size()), buffers.data());

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	void FrameBufferManager::ResetFramebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
