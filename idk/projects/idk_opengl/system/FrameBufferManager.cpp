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
	void FrameBufferManager::SetRenderTarget(const RscHandle<OpenGLCubemap>& target, bool for_convolution)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo_id);
		if (for_convolution)
		{
			glViewport(0, 0, 32, 32);// set texture targets
			auto ids = target->ConvolutedID();
			glBindTexture(GL_TEXTURE_CUBE_MAP, ids[0]);

			vector<GLenum> buffers{GL_COLOR_ATTACHMENT0};
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ids[0], 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
			GL_CHECK();
			glDrawBuffers(s_cast<GLsizei>(buffers.size()), buffers.data());
			GL_CHECK();
			glClear(GL_COLOR_BUFFER_BIT);
		}
		else
		{
			auto sz = target->Size();
			glViewport(0, 0, sz.x, sz.y);

			auto id = r_cast<GLint>(target->ID());

			vector<GLenum> buffers;
			//auto& yolo = *meta.textures[0];
			for (int i = 0; i < 6; ++i)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER,
					GL_COLOR_ATTACHMENT0,
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					id,
					0);
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		CheckFBStatus();
	}

	void FrameBufferManager::SetRenderTarget(RscHandle<FrameBuffer> target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo_id);

		auto& meta = target->GetMeta();
		glViewport(0, 0, meta.size.x, meta.size.y);
		

		// set texture targets
		vector<GLenum> buffers;
		//auto& yolo = *meta.textures[0];
		for (int i = 0; i < meta.textures.size(); ++i)
		{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, s_cast<GLuint>(r_cast<intptr_t>(meta.textures[i]->ID())), 0);
			buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, target->DepthBuffer());
		glDrawBuffers(s_cast<GLsizei>(buffers.size()), buffers.data());

		CheckFBStatus();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void FrameBufferManager::ResetFramebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBufferManager::CheckFBStatus()
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
}
