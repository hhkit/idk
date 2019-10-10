#pragma once
#include <gfx/RenderTarget.h>
#include <glad/glad.h>
namespace idk::ogl
{
	class FrameBuffer
		: public RenderTarget
	{
	public:
		FrameBuffer();
		~FrameBuffer();

		void OnMetaUpdate(const Metadata& newmeta) override;

		GLuint DepthBuffer() const;
	private:
		GLuint depthbuffer = 0;
	};
}