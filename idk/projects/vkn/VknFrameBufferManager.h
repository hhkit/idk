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
	class VknRenderTarget;

	class VknFrameBufferManager
	{
	public:
		VknFrameBufferManager();
		VknFrameBufferManager(VknFrameBufferManager&&);
		VknFrameBufferManager& operator=(VknFrameBufferManager&&);
		~VknFrameBufferManager();

		void SetRenderTarget(RscHandle<VknRenderTarget> target);
		void ResetFramebuffer();

		unique_ptr<VknRenderTarget> Framebuffer();
	private:
		//Framebuffer id or handle (A list of it)
		VknRenderTarget curr_framebuffer{};

		hlp::MemoryAllocator allocator;
		vk::UniqueFence			fence;
	};
}