#pragma once
#include <glad/glad.h>
#include <opengl/resource/OpenGLCubemap.h>
#include <opengl/resource/OpenGLTexture.h>
#include <gfx/Viewport.h>

namespace idk
{
	class RenderTarget;
}

namespace idk::ogl
{
	class OpenGLRenderTarget;
	class OpenGLFrameBuffer;

	class FrameBufferManager
	{
	public:
		FrameBufferManager();
		FrameBufferManager(FrameBufferManager&&);
		FrameBufferManager& operator=(FrameBufferManager&&);
		~FrameBufferManager();

		void SetRenderTarget(const RscHandle<OpenGLCubemap>& target, bool for_convolution = false);
		void SetRenderTarget(RscHandle<OpenGLTexture> target, const std::optional<Viewport>& viewport = std::nullopt);
		void SetRenderTarget(RscHandle<OpenGLRenderTarget> target,const std::optional<Viewport>& viewport = std::nullopt);
		void SetRenderTarget(RscHandle<OpenGLFrameBuffer> target, const std::optional<Viewport>& viewport = std::nullopt);
		void ResetFramebuffer();

		RscHandle<OpenGLTexture> cBufferPickingTexture;
	private:
		GLuint _fbo_id = 0;
		//GLuint _rbo_id = 0;

		void CheckFBStatus();
	};
}
