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
	private:
		GLuint depthbuffer = 0;
	};

	struct OpenGLAttachment :Attachment {};

	class OpenGLFrameBuffer
		: public FrameBuffer
	{
	public:
	private:
	};
}