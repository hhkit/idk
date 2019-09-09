#pragma once
#include <glad/glad.h>

namespace idk
{
	class RenderTarget;
}

namespace idk::ogl
{
	class VknFrameBuffer;

	class FrameBufferManager
	{
	public:
		FrameBufferManager();
		FrameBufferManager(FrameBufferManager&&);
		FrameBufferManager& operator=(FrameBufferManager&&);
		~FrameBufferManager();

		void SetRenderTarget(RscHandle<VknFrameBuffer> target);
		void ResetFramebuffer();
	private:
		GLuint _fbo_id = 0;
	};
}
