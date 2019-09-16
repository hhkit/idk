#pragma once

//#include "idk/res/ResourceHandle.h"
#include <vkn/VulkanView.h>
#include <vkn/VknFrameBuffer.h>

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

		unique_ptr<VknFrameBuffer> Framebuffer();
	private:
		//Framebuffer id or handle (A list of it)
		VknFrameBuffer curr_framebuffer{};

	};
}