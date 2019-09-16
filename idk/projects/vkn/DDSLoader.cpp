#include "pch.h"
#include "DDSLoader.h"
#include <vkn/utils/utils.h>
#include <sstream>
#include <vkn/VknTexture.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <core/core.h>
#include <vkn/VulkanView.h>
namespace idk::vkn
{
	DdsLoader::DdsLoader() :allocator{ *Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().Device(),Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().PDevice() }
	{
		load_fence = Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().View().Device()->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlags{} });
	}
	FileResources DdsLoader::Create(FileHandle path_to_resource)
	{
		auto tex = Core::GetResourceManager().Emplace<VknTexture>();
		auto file = path_to_resource.Open(idk::FS_PERMISSIONS::READ, true);
		std::stringstream strm;
		strm << file.rdbuf();
		DdsFile dds{ strm.str() };
		loader.LoadTexture(*tex, BlockTypeToTextureFormat(dds.File().GetBlockType()), dds.Data(), dds.Dimensions(), allocator, *load_fence);
		return FileResources{ {s_cast<RscHandle<Texture>>(tex) } };
	}

	FileResources DdsLoader::Create(FileHandle path_to_resource, const MetaFile& path_to_meta)
	{
		auto&& tm = path_to_meta.resource_metas[0].get<TextureMeta>();
		//TODO map the format
		//tm.internal_format;
		//TODO send the format and repeat mode in
		return Create(path_to_resource);
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

	size_t DdsFileInternal::NumBytes() const
	{
		//auto pitch = std::max(1u, ((header.width + 3) / 4)) * BlockSize();
		return header.width * header.height;//pitch * s_cast<size_t>(header.height);
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