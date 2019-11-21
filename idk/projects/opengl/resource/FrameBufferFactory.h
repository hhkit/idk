#pragma once
#include <gfx/RenderTarget.h>
#include <res/ResourceFactory.h>
#include <res/compiler/CompiledAssetLoader.h>
#include <opengl/resource/FrameBuffer.h>
#include <gfx/FrameBufferFactory.h>

namespace idk::ogl
{
	class OpenGLRenderTargetFactory
		: public CompiledAssetLoader<OpenGLRenderTarget, OpenGLRenderTarget>
	{
		unique_ptr<RenderTarget> GenerateDefaultResource() override;
		unique_ptr<RenderTarget> Create() override;
	};

	class OpenGLFrameBufferFactory
		: public FrameBufferFactory
	{
		//These are all private. Work with FrameBufferFactory's functions instead.
		unique_ptr<FrameBuffer> GenerateDefaultResource() override;
		unique_ptr<FrameBuffer> Create() override;
		unique_ptr<FrameBuffer> Create(PathHandle h) override;
		//out must be assigned a make unique of the implementation version of attachment
		void CreateAttachment(AttachmentType type, const AttachmentInfo& info, ivec2 size, unique_ptr<Attachment>& out) override;
		void PreReset(FrameBuffer& framebuffer) override;//resets the framebuffer (queue resource for destruction)
		
		void Finalize(FrameBuffer& h_fb) override;
	};
}