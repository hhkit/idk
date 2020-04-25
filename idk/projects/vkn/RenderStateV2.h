#pragma once
#include <vulkan/vulkan.hpp>
#include <vkn/UboManager.h>
#include <vkn/DescriptorsManager.h>
#include <vkn/utils/PresentationSignals.h>
#include <vkn/RenderUtil.h>
#include <vkn/CubemapRenderer.h>
#include <vkn/DeferredPass.h>
#include <meta/stl_hack.h>

namespace idk::vkn
{
struct RenderStateV2
{
	vk::UniqueCommandPool cmd_pool;
	vk::UniqueCommandBuffer cmd_buffer;
	UboManager ubo_manager;//Should belong to each thread group.

	PresentationSignals signal;
	DescriptorsManager dpools;

	//Cubemap renderer here
	//CubemapRenderer skyboxRenderer;

	DeferredPass deferred_pass;

	bool has_commands = false;
	void FlagRendered() { has_commands = true; }
	void Reset();

	const vk::CommandBuffer& CommandBuffer()const;
};
}
MARK_NON_COPY_CTORABLE(idk::vkn::RenderStateV2)