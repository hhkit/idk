#pragma once
#include <vkn/VknRenderTarget.h>
#include <res/ResourceFactory.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
#include <res/SaveableResourceLoader.h>
#include <gfx/RenderTargetFactory.h>
namespace idk::vkn
{

	class VknRenderTargetFactory
		: public RenderTargetFactory
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
	using VknFrameBufferLoader= SaveableResourceLoader<VknRenderTarget>;
	//class VknFrameBufferLoader
	//	: public SaveableResourceLoader<VknRenderTarget>
	//{
	//	using base_t = SaveableResourceLoader<VknRenderTarget>;
	//public:
	//	ResourceBundle LoadFile(PathHandle filepath) override;
	//};
}