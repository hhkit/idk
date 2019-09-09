#pragma once
#include <gfx/RenderTarget.h>
#include <glad/glad.h>
namespace idk::ogl
{
	class VknFrameBuffer
		: public RenderTarget
	{
	public:
		VknFrameBuffer();
		~VknFrameBuffer();

		void OnMetaUpdate(const Metadata& newmeta) override;

		GLuint DepthBuffer() const;
	private:
		GLuint depthbuffer = 0;
	};
}