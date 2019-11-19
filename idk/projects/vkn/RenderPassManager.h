#pragma once
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{
	struct RenderPassConfig
	{
		
	};

	struct RenderPassManager
	{

		vk::RenderPass GetRenderPass(const RenderPassConfig& rpc)
		{
			vk::Device device;
			vk::RenderPassCreateInfo rpci
			{
				{},
			};
			device.createRenderPass()
		}

	};

}