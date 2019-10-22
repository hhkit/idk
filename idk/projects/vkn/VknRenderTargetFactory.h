#pragma once
#include <gfx/RenderTarget.h>
#include <res/ResourceFactory.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
namespace idk::vkn
{
	class VknRenderTargetFactory
		: public ResourceFactory<RenderTarget>
	{
	public:
		VknRenderTargetFactory();
		unique_ptr<RenderTarget> GenerateDefaultResource() override;
		unique_ptr<RenderTarget> Create() override;
		//Do not call this on multiple concurrent threads.
		hlp::MemoryAllocator& GetAllocator() { return allocator; }
		//Do not call this on multiple concurrent threads.
		vk::Fence GetFence() { return *fence; }
		//unique_ptr<RenderTarget> Create(PathHandle fh) override;
		//unique_ptr<RenderTarget> Create(PathHandle filepath, const RenderTarget::Metadata& m);
	private:
		hlp::MemoryAllocator allocator;
		vk::UniqueFence			fence;
	};

	class VknFrameBufferLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle filepath, const MetaBundle& m);
	};
}