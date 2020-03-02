#pragma once
#include <vkn/VknTextureView.h>
#include <vkn/AttachmentDescription.h>
#include <forward_list>
#include <future>
namespace idk::vkn
{

	struct TexturePool
	{
	public:
		TexturePool();
		TexturePool(const TexturePool&) = delete;;
		TexturePool(TexturePool&&);
		TexturePool& operator=(const TexturePool&) = delete;;
		TexturePool& operator=(TexturePool&&);
		~TexturePool();
		VknTextureView allocate(const TextureDescription& description);
		std::future<VknTextureView> allocate_async(const TextureDescription& description);
		void collate_async();
		void reset_allocations();
	private:
		VknTextureView create(const TextureDescription& description);
		std::future<VknTextureView> create_async(const TextureDescription& description);
		//search for a free texture that matches the description
		std::optional<VknTextureView> free_match(const TextureDescription& desc);

		struct Pimpl;
		std::unique_ptr<Pimpl> _pimpl;

		vector<unique_ptr<VknTexture>> allocated_textures;
		//potentially use unordered_multimap or unordered_map of forward_lists
		std::forward_list<unique_ptr<VknTexture>> free_textures;

	};
}