#include "pch.h"
#include "VknFrameBufferManager.h"
#include <gfx/RenderTarget.h>
#include <vkn/VknFrameBuffer.h>
#include <vkn/VknTexture.h>

#include <iostream>


namespace idk::vkn
{
	VknFrameBufferManager::VknFrameBufferManager()
	{
		//glGenFramebuffers(1, &_fbo_id);
		//Gen a framebuffer
	}

	VknFrameBufferManager::VknFrameBufferManager(VknFrameBufferManager&& rhs)
	{

		/*rhs._fbo_id = 0;*/
		//Should be to coopy the ids

	}
	VknFrameBufferManager& VknFrameBufferManager::operator=(VknFrameBufferManager&& rhs)
	{
		/*std::swap(_fbo_id, rhs._fbo_id);*/

		//Same thing
		return *this;
	}
	VknFrameBufferManager::~VknFrameBufferManager()
	{
		//glDeleteFramebuffers(1, &_fbo_id);
	}
	void VknFrameBufferManager::SetRenderTarget(RscHandle<VknFrameBuffer> target)
	{
		//Set render target is a impt function

		//glBindFramebuffer(GL_FRAMEBUFFER, _fbo_id);

		//auto& meta = target->GetMeta();
		//glViewport(0, 0, meta.size.x, meta.size.y);


		//// set texture targets
		//vector<GLenum> buffers;
		//auto& yolo = *meta.textures[0];
		//for (int i = 0; i < meta.textures.size(); ++i)
		//{
		//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, (GLuint)meta.textures[i]->ID(), 0);
		//	buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		//}
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, (GLuint)target->DepthBuffer());
		//glDrawBuffers(s_cast<GLsizei>(buffers.size()), buffers.data());

		//{
		//	auto fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		//	switch (fb_status)
		//	{
		//	case GL_FRAMEBUFFER_UNDEFINED:                     std::cout << "undefined\n";       break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         std::cout << "incomplete\n";		 break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: std::cout << "missing\n";		 break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        std::cout << "incomplete draw\n"; break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        std::cout << "incomplete read\n"; break;
		//	case GL_FRAMEBUFFER_UNSUPPORTED:                   std::cout << "unsupported\n";	 break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        std::cout << "multisample\n";	 break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:      std::cout << "layer target\n";	 break;
		//	}
		//	assert(fb_status == GL_FRAMEBUFFER_COMPLETE);
		//}
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}
	void VknFrameBufferManager::ResetFramebuffer()
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
