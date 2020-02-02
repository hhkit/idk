#include "pch.h"
#include "TexturePool.h"
#include <vkn/VulkanView.h>
#include <vkn/VknTextureLoader.h>
namespace idk::vkn
{
	struct TexturePool::Pimpl
	{
		hlp::MemoryAllocator allocator;
		vk::UniqueFence uload_fence;
		vk::Fence load_fence;
		Pimpl() :uload_fence{ View().Device()->createFenceUnique(vk::FenceCreateInfo{}) }, load_fence{ *uload_fence }
		{

		}

	};
	TexturePool::TexturePool() :_pimpl{ std::make_unique<Pimpl>() }
	{

	};
	TexturePool::TexturePool(TexturePool&&) = default;
	TexturePool& TexturePool::operator=(TexturePool&&) = default;
	TexturePool::~TexturePool() = default;
#pragma optimize("",off)
	bool desc_match(TextureDescription desc, VknTexture& texture)
	{
		return vk::ImageAspectFlags{ desc.aspect } == texture.img_aspect
			&&
			desc.format == texture.format
			&&
			(texture.usage & desc.usage) == desc.usage
			&&
			texture.Size() == desc.size;
	}

	std::optional<VknTextureView> TexturePool::free_match(const TextureDescription& desc)
	{
		std::optional<VknTextureView> result{};
		for (auto pitr = free_textures.before_begin(),itr=pitr, end = free_textures.end(); ++itr != end;)
		{
			if (desc_match(desc, **itr))
			{
				result = { **itr };
				allocated_textures.emplace_back(std::move(*itr));
				free_textures.erase_after(pitr);
				break;
			}
			pitr = itr;
		}
		return result;
	}

	VknTextureView TexturePool::allocate(const TextureDescription& desc)
	{
		auto free_result = free_match(desc);
		if (!free_result)
		{
			free_result = create(desc);
		}
		return *free_result;
	}
	VknTextureView TexturePool::create(const TextureDescription& desc)
	{
		auto rsc_ptr = std::make_unique<VknTexture>();

		TexCreateInfo tci{};
		tci.internal_format = desc.format;
		tci.width = desc.size.x;
		tci.height = desc.size.y;
		tci.aspect = desc.aspect;
		tci.layout = vk::ImageLayout::eUndefined;
		tci.mipmap_level = desc.mipmap_level;
		tci.image_usage = desc.usage;
		TextureLoader loader;
		loader.LoadTexture(*rsc_ptr, _pimpl->allocator, _pimpl->load_fence, {}, tci, {});
		auto& tex = allocated_textures.emplace_back(std::move(rsc_ptr));
		return *tex;
	}

	void TexturePool::reset_allocations()
	{
		for (auto& alloced : allocated_textures)
		{
			free_textures.emplace_front(std::move(alloced));
		}
		allocated_textures.clear();
	}

}