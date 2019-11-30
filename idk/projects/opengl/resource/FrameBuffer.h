#pragma once
#include <gfx/RenderTarget.h>
#include <glad/glad.h>
#include <gfx/Framebuffer.h>
namespace idk::ogl
{
	class OpenGLRenderTarget
		: public RenderTarget
	{
	public:
		OpenGLRenderTarget();

		void OnFinalize() override;

		GLuint DepthBuffer() const;

		RscHandle<FrameBuffer> DeferredBufferPBRMetallic() const;
	private:
		GLuint depthbuffer = 0;

		RscHandle<FrameBuffer> pbr_metallic_gbuffer;
	};

	struct OpenGLAttachment :Attachment {};

	class OpenGLFrameBuffer
		: public FrameBuffer
	{
	public:
	private:
	};
}