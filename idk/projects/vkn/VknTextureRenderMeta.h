#pragma once
#include <optional>

#include <gfx/TextureRenderMeta.h>
#include <gfx/TextureInternalFormat.h>
#include <vulkan/vulkan.hpp>
#include <vkn/utils/Flags.h>

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

	hash_table<TextureInternalFormat, vk::Format>		CFormatMap();
	hash_table<FilterMode::_enum, vk::Filter>			FilterMap();
	hash_table<UVMode::_enum, vk::SamplerAddressMode>	UVModeMap();

	vk::Format    MapFormat(TextureFormat tf);
	vk::Format    MapFormat(TextureInternalFormat tf);
	TextureFormat MapFormat(vk::Format tf);
	vk::Format    UnSrgb(vk::Format);
	vk::Format    ToSrgb(vk::Format f);
	vk::CompareOp MapCompareOp(CompareOp compare_op);
	void		  PrintFormatBlitCompatibility();

	enum class BlitCompatFlagBits : uint32_t
	{
		eDstLinear = 0b0001,
		eDstOptimal = 0b0010,
		eSrcLinear = 0b0100,
		eSrcOptimal = 0b1000,
	};

	enum class BlitCompatUsageMasks : uint32_t
	{
		eDst = 0b0011,
		eSrc = 0b1100
	};

	enum class BlitCompatTileMasks : uint32_t
	{
		eLinear = 0b0101,
		eOptimal = 0b1010
	};

	using BlitCompatFlag_t = uint32_t;
	using BlitCompatFlags = Flags <BlitCompatFlagBits, BlitCompatFlag_t>;


	inline BlitCompatFlags CompatMask(BlitCompatTileMasks t_mask, BlitCompatUsageMasks u_mask)
	{
		return BlitCompatFlags{ s_cast<BlitCompatFlag_t>(t_mask) & s_cast<BlitCompatFlag_t>(u_mask) };
	}

	bool BlitCompatible(vk::Format format, BlitCompatUsageMasks usage);

	std::optional<vk::ImageTiling> GetNearestTiling(vk::Format format, BlitCompatTileMasks tiling, BlitCompatUsageMasks usage);
	std::optional<vk::Format> NearestBlittableFormat(vk::Format format, BlitCompatUsageMasks usage);


	inline BlitCompatFlags operator&(BlitCompatUsageMasks lhs, BlitCompatTileMasks rhs) { return BlitCompatFlags{ s_cast<BlitCompatFlag_t>(lhs) & s_cast<BlitCompatFlag_t>(rhs) }; }
	inline BlitCompatFlags operator&(BlitCompatTileMasks lhs, BlitCompatUsageMasks rhs) { return BlitCompatFlags{ s_cast<BlitCompatFlag_t>(lhs) & s_cast<BlitCompatFlag_t>(rhs) }; }
	inline BlitCompatFlags operator|(BlitCompatUsageMasks lhs, BlitCompatTileMasks rhs) { return BlitCompatFlags{ s_cast<BlitCompatFlag_t>(lhs) | s_cast<BlitCompatFlag_t>(rhs) }; }
	inline BlitCompatFlags operator|(BlitCompatTileMasks lhs, BlitCompatUsageMasks rhs) { return BlitCompatFlags{ s_cast<BlitCompatFlag_t>(lhs) | s_cast<BlitCompatFlag_t>(rhs) }; }

	inline BlitCompatFlags operator&(BlitCompatFlags lhs, BlitCompatTileMasks rhs) { return BlitCompatFlags{ s_cast<BlitCompatFlag_t>(lhs) & s_cast<BlitCompatFlag_t>(rhs) }; }
	inline BlitCompatFlags operator&(BlitCompatTileMasks lhs, BlitCompatFlags rhs) { return BlitCompatFlags{ s_cast<BlitCompatFlag_t>(lhs) & s_cast<BlitCompatFlag_t>(rhs) }; }
	inline BlitCompatFlags operator|(BlitCompatFlags lhs, BlitCompatTileMasks rhs) { return BlitCompatFlags{ s_cast<BlitCompatFlag_t>(lhs) | s_cast<BlitCompatFlag_t>(rhs) }; }
	inline BlitCompatFlags operator|(BlitCompatTileMasks lhs, BlitCompatFlags rhs) { return BlitCompatFlags{ s_cast<BlitCompatFlag_t>(lhs) | s_cast<BlitCompatFlag_t>(rhs) }; }

	inline BlitCompatFlags operator&(BlitCompatUsageMasks lhs, BlitCompatFlags rhs) { return BlitCompatFlags{ s_cast<BlitCompatFlag_t>(lhs) & s_cast<BlitCompatFlag_t>(rhs) }; }
	inline BlitCompatFlags operator&(BlitCompatFlags lhs, BlitCompatUsageMasks rhs) { return BlitCompatFlags{ s_cast<BlitCompatFlag_t>(lhs) & s_cast<BlitCompatFlag_t>(rhs) }; }
	inline BlitCompatFlags operator|(BlitCompatUsageMasks lhs, BlitCompatFlags rhs) { return BlitCompatFlags{ s_cast<BlitCompatFlag_t>(lhs) | s_cast<BlitCompatFlag_t>(rhs) }; }
	inline BlitCompatFlags operator|(BlitCompatFlags lhs, BlitCompatUsageMasks rhs) { return BlitCompatFlags{ s_cast<BlitCompatFlag_t>(lhs) | s_cast<BlitCompatFlag_t>(rhs) }; }
}