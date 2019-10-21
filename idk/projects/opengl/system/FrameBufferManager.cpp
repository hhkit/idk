#include "pch.h"
#include "FrameBufferManager.h"
#include <gfx/RenderTarget.h>
#include <opengl/resource/FrameBuffer.h>
#include <opengl/resource/OpenGLTexture.h>

#include <iostream>


namespace idk::ogl
{
	FrameBufferManager::FrameBufferManager()
	{
		glGenFramebuffers(1, &_fbo_id);
		//glGenRenderbuffers(1, &_rbo_id);
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
			const auto ids = target->ConvolutedID();
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
			const auto sz = target->Size();
			glViewport(0, 0, sz.x, sz.y);

            auto id = static_cast<GLint>(reinterpret_cast<ptrdiff_t>(target->ID()));

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

	void FrameBufferManager::SetRenderTarget(RscHandle<OpenGLTexture> target)
	{

		const auto sz = target->Size();
		glViewport(0, 0, sz.x, sz.y);
		if (target->GetMeta().internal_format == ColorFormat::DEPTH_COMPONENT)
		{
			//glBindRenderbuffer(GL_RENDERBUFFER, _rbo_id);
			//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024,1024);
			glBindFramebuffer(GL_FRAMEBUFFER, _fbo_id);
			glDepthFunc(GL_LEQUAL);

			//glEnable(GL_DEPTH_TEST);

			target->Bind();
			//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo_id);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, s_cast<GLuint>(r_cast<intptr_t>(target->ID())), 0);
			GLuint buffers[] = { GL_DEPTH_ATTACHMENT };
			//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
			glDrawBuffers(1, buffers);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, _fbo_id);
			TextureMeta mm = target->GetMeta();
			if (mm.internal_format != ColorFormat::RGBAF_16)
			{
				mm.internal_format = ColorFormat::RGBAF_16;
				target->SetMeta(mm);
			}
			target->Bind();

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, s_cast<GLuint>(r_cast<intptr_t>(target->ID())), 0);
			const GLuint buffers[] = { GL_COLOR_ATTACHMENT0 };
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
			glDrawBuffers(1, buffers);
		}


		CheckFBStatus();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}
	void FrameBufferManager::SetRenderTarget(RscHandle<OpenGLRenderTarget> target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo_id);

		auto& meta = target->GetMeta();
		glViewport(0, 0, meta.size.x, meta.size.y);
		

		// set texture targets
		vector<GLenum> buffers;
		//auto& yolo = *meta.textures[0];
		//for (int i = 0; i < std::size(meta.textures); ++i)
		int i = 0;
		{
			TextureMeta mm = meta.textures[i].as<OpenGLTexture>().GetMeta();
			if (mm.internal_format != ColorFormat::RGBAF_16)
			{
				mm.internal_format = ColorFormat::RGBAF_16;
				meta.textures[i].as<OpenGLTexture>().SetMeta(mm);
			}
			meta.textures[i].as<OpenGLTexture>().Bind();
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, s_cast<GLuint>(r_cast<intptr_t>(meta.textures[i]->ID())), 0);
			buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, target->DepthBuffer());
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, s_cast<GLuint>(r_cast<intptr_t>(meta.textures[RenderTarget::kDepthIndex]->ID())), 0);
		glDrawBuffers(s_cast<GLsizei>(buffers.size()), buffers.data());

		CheckFBStatus();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void FrameBufferManager::SetRenderTarget(RscHandle<OpenGLFrameBuffer> target)
	{
		IDK_ASSERT_MSG(&*target,"Attempting to use a default framebuffer. Default framebuffer should not be used.");//make sure it's not null. 
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo_id);

		auto size = target->Size();
		glViewport(0, 0, size.x, size.y);


		// set texture targets
		vector<GLenum> buffers;
		//auto& yolo = *meta.textures[0];

		for (int i = 0; i < target->NumColorAttachments(); ++i)
		{
			auto& attachment = target->GetAttachment(i);
			auto& tex = attachment.buffer.as<OpenGLTexture>();
			TextureMeta mm = tex.GetMeta();
			if (mm.internal_format != ColorFormat::RGBAF_16)
			{
				mm.internal_format = ColorFormat::RGBAF_16;
				tex.SetMeta(mm);
			}
			tex.Bind();
			auto attachment_index = GL_COLOR_ATTACHMENT0 + i;
			glFramebufferTexture(GL_FRAMEBUFFER, attachment_index, s_cast<GLuint>(r_cast<intptr_t>(tex.ID())), 0);
			buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, target->DepthBuffer());
		if (target->HasDepthAttachment())
		{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, s_cast<GLuint>(r_cast<intptr_t>(target->DepthAttachment().buffer->ID())), 0);
		}
		if (!target->NumColorAttachments())
		{
			//GLint max_draw =0;
			//glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max_draw);
			glFramebufferParameteri(_fbo_id, GL_FRAMEBUFFER_DEFAULT_WIDTH , target->Size().x);
			glFramebufferParameteri(_fbo_id, GL_FRAMEBUFFER_DEFAULT_HEIGHT, target->Size().y);
			glDrawBuffer(GL_NONE);
		}else
		{
			glDrawBuffers(s_cast<GLsizei>(buffers.size()), buffers.data());
		}

		CheckFBStatus();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void FrameBufferManager::ResetFramebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBufferManager::CheckFBStatus()
	{
		const auto fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

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
