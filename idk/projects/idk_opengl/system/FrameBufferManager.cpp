#include "pch.h"
#include "FrameBufferManager.h"
#include <gfx/RenderTarget.h>
#include <idk_opengl/resource/FrameBuffer.h>
#include <idk_opengl/resource/OpenGLTexture.h>

#include <iostream>


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
		auto& yolo = *meta.textures[0];
		for (int i = 0; i < meta.textures.size(); ++i)
		{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, (GLuint)meta.textures[i]->ID(), 0);
			buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, (GLuint)target->DepthBuffer());
		glDrawBuffers(s_cast<GLsizei>(buffers.size()), buffers.data());

		{
			auto fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			
			switch (fb_status)
			{
			case GL_FRAMEBUFFER_UNDEFINED:                     std::cout << "undefined\n";       break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         std::cout << "incomplete\n";		 break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: std::cout << "missing\n";		 break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        std::cout << "incomplete draw\n"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        std::cout << "incomplete read\n"; break;
			case GL_FRAMEBUFFER_UNSUPPORTED:                   std::cout << "unsupported\n";	 break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        std::cout << "multisample\n";	 break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:      std::cout << "layer target\n";	 break;
			}
			assert(fb_status == GL_FRAMEBUFFER_COMPLETE);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	}
	void FrameBufferManager::ResetFramebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
