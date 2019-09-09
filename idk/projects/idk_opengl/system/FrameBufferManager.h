#pragma once
#include <glad/glad.h>

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

		void SetRenderTarget(RscHandle<FrameBuffer> target);
		void ResetFramebuffer();
	private:
		GLuint _fbo_id = 0;
	};
}
