#pragma once
#include <glad/glad.h>
#include <opengl/resource/OpenGLCubemap.h>
#include <opengl/resource/OpenGLTexture.h>
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
		void SetRenderTarget(RscHandle<OpenGLTexture> target);
		void SetRenderTarget(RscHandle<OpenGLRenderTarget> target);
		void SetRenderTarget(RscHandle<OpenGLFrameBuffer> target);
		void ResetFramebuffer();
		
		RscHandle<OpenGLTexture> cBufferPickingTexture;
		RscHandle<OpenGLFrameBuffer> pickingBuffer;

		
	private:
		GLuint _fbo_id = 0;
		//GLuint _rbo_id = 0;

		void CheckFBStatus();
	};
}
