#pragma once
#include <gfx/RenderTarget.h>
#include <res/ResourceFactory.h>

#include <gfx/FrameBufferFactory.h>

namespace idk::ogl
{
	class OpenGLRenderTargetFactory
		: public ResourceFactory<RenderTarget>
	{
		unique_ptr<RenderTarget> GenerateDefaultResource() override;
		unique_ptr<RenderTarget> Create() override;
	};

	class FrameBufferLoader
		: public IFileLoader
	{
		ResourceBundle LoadFile(PathHandle fh) override;
		ResourceBundle LoadFile(PathHandle filepath, const MetaBundle& m) override;
	};
	class OpenGLFrameBufferFactory
		: public FrameBufferFactory
	{
		//These are all private. Work with FrameBufferFactory's functions instead.
		unique_ptr<FrameBuffer> GenerateDefaultResource() override;
		unique_ptr<FrameBuffer> Create() override;
		//out must be assigned a make unique of the implementation version of attachment
		void CreateAttachment(AttachmentType type, const AttachmentInfo& info, ivec2 size, unique_ptr<Attachment>& out) override;
		void PreReset(FrameBuffer& framebuffer) override;//resets the framebuffer (queue resource for destruction)
		
		void Finalize(FrameBuffer& h_fb) override;
	};
}