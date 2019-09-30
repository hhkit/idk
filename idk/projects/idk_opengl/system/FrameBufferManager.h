#pragma once
#include <glad/glad.h>
#include <idk_opengl/resource/OpenGLCubemap.h>

namespace idk
{
	class RenderTarget;
}

namespace idk::ogl
{
	class FrameBuffer;

	class FrameBufferManager
	{
	public:
		FrameBufferManager();
		FrameBufferManager(FrameBufferManager&&);
		FrameBufferManager& operator=(FrameBufferManager&&);
		~FrameBufferManager();

		void SetRenderTarget(const RscHandle<OpenGLCubemap>& target, bool for_convolution = false);
		void SetRenderTarget(RscHandle<FrameBuffer> target);
		void ResetFramebuffer();
	private:
		GLuint _fbo_id = 0;

		void CheckFBStatus();
	};
}
