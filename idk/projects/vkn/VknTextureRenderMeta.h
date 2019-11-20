#pragma once
#include <gfx/TextureRenderMeta.h>
#include <vulkan/vulkan.hpp>

namespace idk::vkn
{
	enum class TextureFormat
	{
		eD16Unorm,
		eRGBA32,
		eBGRA32,
		eBC1,
		eBC2,
		eBC3,
		eBC4,
		eBC5,
	};

	enum class CompareOp
	{
		eNever,
		eLess,
		eEqual,
		eLessOrEqual,
		eGreater,
		eNotEqual,
		eGreaterOrEqual,
		eAlways
	};

	hash_table<ColorFormat::_enum, vk::Format>			CFormatMap();
	hash_table<FilterMode::_enum, vk::Filter>			FilterMap();
	hash_table<UVMode::_enum, vk::SamplerAddressMode>	UVModeMap();

	vk::Format    MapFormat(TextureFormat tf);
	vk::Format    MapFormat(ColorFormat tf);
	TextureFormat MapFormat(vk::Format    tf);
	vk::Format    UnSrgb(vk::Format);
	vk::Format    ToSrgb(vk::Format f);
	vk::CompareOp MapCompareOp(CompareOp compare_op);
	void		  PrintFormatBlitCompatibility();
}