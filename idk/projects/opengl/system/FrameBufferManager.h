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
	class FrameBuffer;

	class FrameBufferManager
	{
	public:
		FrameBufferManager();
		FrameBufferManager(FrameBufferManager&&);
		FrameBufferManager& operator=(FrameBufferManager&&);
		~FrameBufferManager();

		void SetRenderTarget(const RscHandle<OpenGLCubemap>& target, bool for_convolution = false);
		void SetRenderTarget(RscHandle<FrameBuffer> target, vector<OpenGLTexture> additionalCustomAttachmentList);
		void SetRenderTarget(RscHandle<OpenGLTexture> target);
		void SetRenderTarget(RscHandle<FrameBuffer> target);
		void ResetFramebuffer();

		RscHandle<OpenGLTexture> cBufferPickingTexture;
	private:
		GLuint _fbo_id = 0;
		//GLuint _rbo_id = 0;

		void CheckFBStatus();
	};
}
