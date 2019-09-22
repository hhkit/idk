#pragma once
#include <gfx/RenderTarget.h>
#include <res/ResourceFactory.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{
	class VknFrameBufferFactory
		: public ResourceFactory<RenderTarget>
	{
	public:
		VknFrameBufferFactory();
		unique_ptr<RenderTarget> GenerateDefaultResource() override;
		unique_ptr<RenderTarget> Create() override;
		unique_ptr<RenderTarget> Create(PathHandle fh) override;
		unique_ptr<RenderTarget> Create(PathHandle filepath, const RenderTarget::Metadata& m);
	private:
		hlp::MemoryAllocator allocator;
		vk::UniqueFence			fence;
	};
}