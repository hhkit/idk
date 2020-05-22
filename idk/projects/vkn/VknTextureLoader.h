#pragma once
#include <idk.h>
#include <vkn/MemoryAllocator.h>
#include <vulkan/vulkan.hpp>
#include <vkn/VknTexture.h>

#include <vkn/VknTextureRenderMeta.h>
#include <vkn/FencePool.h>
#include <parallel/ThreadPool.h>

#include <vkn/AsyncTexLoadInfo.h>


namespace idk
{
	struct CompiledTexture;
}

namespace idk::vkn
{
	struct VknTexture;
	struct VknTextureData;

	TexCreateInfo ColorBufferTexInfo(uint32_t width, uint32_t height);
	TexCreateInfo DepthBufferTexInfo(uint32_t width, uint32_t height);
	TexCreateInfo StencilBufferTexInfo(uint32_t width, uint32_t height);
	TexCreateInfo Depth3DBufferTexInfo(uint32_t width, uint32_t height);
	class TextureLoader
	{
	public:
		//Will override TexCreateInfo's format if TextureOptions is set
		struct SubmissionObjs
		{
			std::optional<vk::CommandBuffer> cmd_buffer;
			vk::Fence load_fence;
			bool end_and_submit = true;
		};
		std::optional< std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory>> LoadTexture(SubmissionObjs sub,VknTextureData& texture, hlp::MemoryAllocator& allocator,std::optional<TextureOptions> ooptional, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid = {});
		void LoadTexture(VknTexture& texture, hlp::MemoryAllocator& allocator, vk::Fence load_fence, std::optional<TextureOptions> ooptional, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid = {});
		void LoadTexture(VknTextureData& texture, hlp::MemoryAllocator& allocator, vk::Fence load_fence,std::optional<TextureOptions> ooptional, const TexCreateInfo& load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid = {});

		struct AsyncResult
		{
			FenceObj fence;
			CmdBufferObj cmd_buffer;
			std::optional< std::pair<vk::UniqueBuffer, vk::UniqueDeviceMemory>> staging;
			bool ready()const;
		};

		mt::ThreadPool::Future<void> LoadTextureAsync(VknTexture& texture, hlp::MemoryAllocator& allocator, FencePool& load_fence, CmdBufferPool& cmd_buffers, std::optional<TextureOptions> ooptional, TexCreateInfo load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid = {}    ,AsyncResult* result=nullptr);
		mt::ThreadPool::Future<void> LoadTextureAsync(VknTextureData& texture, hlp::MemoryAllocator& allocator, FencePool& load_fence, CmdBufferPool& cmd_buffers, std::optional<TextureOptions> ooptional, TexCreateInfo load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid = {},AsyncResult* result=nullptr);

		hlp::SimpleLock lock;
		int abc = 0;
	private:
		mt::ThreadPool::Future<void> LoadTextureAsync(std::variant<VknTextureData*,VknTexture*> texture, hlp::MemoryAllocator& allocator, FencePool& load_fence, CmdBufferPool& cmd_buffers, std::optional<TextureOptions> ooptional, TexCreateInfo load_info, std::optional<InputTexInfo> in_info, std::optional<Guid> guid = {}, AsyncResult* result = nullptr);
	};

	struct ImageViewInfo
	{
		uint32_t base_mip_level = 0;
		uint32_t level_count = 1;
		uint32_t base_array_layer = 0;
		uint32_t array_layer_count = 1;
		vk::ImageViewType type = vk::ImageViewType::e2D;
		vk::ComponentMapping component_mapping{};
	};

	vk::UniqueImageView CreateImageView2D(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspect, ImageViewInfo = {});
}