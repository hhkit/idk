#include "pch.h"
#include "DDSLoader.h"
#include <vkn/utils/utils.h>
#include <sstream>
#include <vkn/VknTexture.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <core/core.h>
#include <vkn/VulkanView.h>
#include <res/MetaBundle.h>
#include <res/ResourceManager.inl>

namespace idk::vkn
{
	DdsLoader::DdsLoader() :allocator{ *Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().Device(),Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().PDevice() }
	{
		load_fence = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlags{} });
	}

	hash_table<vk::Format, vk::Format> DecompressMap()
	{
		return hash_table<vk::Format, vk::Format>
		{
			{ vk::Format::eBc1RgbUnormBlock  , vk::Format::eR8G8B8A8Unorm },
			{ vk::Format::eBc2UnormBlock     , vk::Format::eR8G8B8A8Unorm },
			{ vk::Format::eBc3UnormBlock     , vk::Format::eR8G8B8A8Unorm },
			{ vk::Format::eBc1RgbaUnormBlock , vk::Format::eR8G8B8A8Unorm },
		};

	}
	vk::Format ToDecompressed(vk::Format format)
	{
		static const auto map = DecompressMap();
		vk::Format result = format;
		auto itr = map.find(format);
		if (itr != map.end())
			result = itr->second;
		return result;
	}
	uint32_t validate_mipmap_level(uint32_t m_lvl, uint32_t w, uint32_t h)
	{
		uint32_t d = std::max(w, h);//padded out
		uint32_t i = m_lvl;
		//while d is too small to support i mipmaps
		while (i > 0 && !((d >> i)>=4))--i; //Decrease mipmap count
		return i;
	}
	ResourceBundle DdsLoader::LoadFile(PathHandle path_to_resource, const MetaBundle& bundle)
	{
		//auto&& tm = *path_to_meta.metadatas[0].GetMeta<Texture>();
		//TODO map the format
		//tm.internal_format;
		//TODO send the format and repeat mode in
		auto meta = bundle.FetchMeta<Texture>();
		auto tex = meta 
			? Core::GetResourceManager().LoaderEmplaceResource<VknTexture>(meta->guid)
			: Core::GetResourceManager().LoaderEmplaceResource<VknTexture>();

		auto file = path_to_resource.Open(idk::FS_PERMISSIONS::READ, true);
		std::stringstream strm;
		strm << file.rdbuf();
        DdsFile dds{ string{strm.str()} };
		TextureOptions to;
		if (meta)
		{
			auto load = meta->GetMeta<Texture>();
			if (load)
				to = *load;
		}

		InputTexInfo iti;
		iti.data = dds.Data().data();
		iti.len = dds.Data().length();
		iti.format = MapFormat(BlockTypeToTextureFormat(dds.File().GetBlockType()));
		TexCreateInfo tci;
		tci.aspect = vk::ImageAspectFlagBits::eColor;
		tci.width = dds.Dimensions().x;
		tci.height = dds.Dimensions().y;
		tci.mipmap_level = std::max(dds.File().header.mip_map_count,0u);
		tci.mipmap_level = validate_mipmap_level(tci.mipmap_level, tci.width,tci.height);
		tci.internal_format = MapFormat(to.internal_format);
		tci.image_usage = vk::ImageUsageFlagBits::eSampled;
		
		bool linearize = !IsSrgb(to.internal_format);
		if (linearize)
		{
			tci.internal_format = ToDecompressed(UnSrgb(MapFormat(to.internal_format)));
		}
		loader.LoadTexture(*tex, allocator, *load_fence,to,tci,iti);
		return tex;
	}


	//To represent the start of the data.

	BlockType DdsFileInternal::GetBlockType() const
	{
		static hash_table<string, BlockType> map =
		{
			{ "DXT1",BlockType::eBC1 },
		{ "DXT2",BlockType::eBC2 },
		{ "DXT3",BlockType::eBC2 },
		{ "DXT4",BlockType::eBC3 },
		{ "DXT5",BlockType::eBC3 },
		};
		string format_str = GetFormatString();
		auto itr = map.find(format_str);
		return (itr == map.end()) ? BlockType::eNone : itr->second;
	}

	const char(&DdsFileInternal::GetFormatString() const)[4]
	{
		return r_cast<const char(&)[4]>(header.pf.fourCC);
	}

	uint32_t DdsFileInternal::BlockSize() const
	{
		uint32_t block_size = 0;
		switch (GetBlockType())
		{
		case BlockType::eNone:
			break;
		case BlockType::eBC1:
		case BlockType::eBC4:
			block_size = 8;
			break;
		default:
			block_size = 16;
			break;
		}
		return block_size;
	}
	size_t ceil_div(size_t n, size_t d)
	{
		return n / d + ((n % d) ? 1 : 0);
	}
	size_t align(size_t v, size_t a)
	{
		const auto mod = v % a;
		// v + (a-mod)%mod;
		return v + (mod) ? (a-mod) : 0;
	}
	size_t compressed_size([[maybe_unused]]BlockType type, size_t width, size_t height)
	{
		size_t result = width * height;
		const size_t block_size = 4;
		width  = ceil_div(width , block_size );
		height = ceil_div(height, block_size );
		switch (type)
		{
		case BlockType::eBC1:
		case BlockType::eBC4:
			result = (width * height)*8; // 8 bytes per chunk
			break;
		
		case BlockType::eBC2:
		case BlockType::eBC3:
		case BlockType::eBC5:
			result = (width * height)*16; //16 bytes per chunk
			break;
		default:
			break;
		}
		return result;
	}
	size_t DdsFileInternal::NumBytes() const
	{
		return compressed_size(this->GetBlockType(), s_cast<size_t>(header.width) , s_cast<size_t>(header.height));
	}

	hash_table<BlockType, TextureFormat> MapBlockTypeToTextureFormat()
	{
		return hash_table<BlockType, TextureFormat>
		{
			{BlockType::eNone, TextureFormat::eRGBA32},
			{ BlockType::eBC1 ,TextureFormat::eBC1 },
			{ BlockType::eBC2 ,TextureFormat::eBC2 },
			{ BlockType::eBC3 ,TextureFormat::eBC3 },
			{ BlockType::eBC4 ,TextureFormat::eBC4 },
			{ BlockType::eBC5 ,TextureFormat::eBC5 },
		};
	}

	TextureFormat BlockTypeToTextureFormat(BlockType tf)
	{
		static auto map = MapBlockTypeToTextureFormat();

		return map.find(tf)->second;
	}

	BlockType TextureFormatToBlockType(TextureFormat tf)
	{
		static auto map = hlp::ReverseMap(MapBlockTypeToTextureFormat());

		return map.find(tf)->second;
	}

}