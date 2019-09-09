#pragma once

//#include "idk/res/ResourceHandle.h"

namespace idk
{
	class RenderTarget;
}

namespace idk::vkn
{
	class VknFrameBuffer;

	class VknFrameBufferManager
	{
	public:
		VknFrameBufferManager();
		VknFrameBufferManager(VknFrameBufferManager&&);
		VknFrameBufferManager& operator=(VknFrameBufferManager&&);
		~VknFrameBufferManager();

		void SetRenderTarget(RscHandle<VknFrameBuffer> target);
		void ResetFramebuffer();
	private:
		//Framebuffer id or handle
	};
}