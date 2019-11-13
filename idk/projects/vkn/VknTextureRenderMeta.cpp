#include "pch.h"
#include <vkn/VknTextureRenderMeta.h>
#include <vkn/utils/utils.h> //ReverseMap
#include <vkn/VulkanView.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <iostream>

namespace idk::vkn {

	hash_table<TextureFormat, vk::Format> FormatMap()
	{
		return hash_table<TextureFormat, vk::Format>
		{
			{TextureFormat::eD16Unorm, vk::Format::eD16Unorm},
			{ TextureFormat::eRGBA32, vk::Format::eR8G8B8A8Unorm },
			{ TextureFormat::eBGRA32, vk::Format::eB8G8R8A8Unorm },
			{ TextureFormat::eBC1,vk::Format::eBc1RgbaSrgbBlock },//Auto srgb
			{ TextureFormat::eBC2,vk::Format::eBc2SrgbBlock },	  //Auto srgb
			{ TextureFormat::eBC3,vk::Format::eBc3SrgbBlock },	  //Auto srgb
			{ TextureFormat::eBC4,vk::Format::eBc4UnormBlock },
			{ TextureFormat::eBC5,vk::Format::eBc5UnormBlock },
		};
	}
	hash_table<ColorFormat::_enum, vk::Format> CFormatMap()
	{
		return hash_table<ColorFormat::_enum, vk::Format>
		{
			{ ColorFormat::_enum::DEPTH_COMPONENT, vk::Format::eD16Unorm},
			{ ColorFormat::_enum::R_8, vk::Format::eR8Unorm },
			{ ColorFormat::_enum::R_16, vk::Format::eR16Unorm },
			{ ColorFormat::_enum::R_32F, vk::Format::eR32Sfloat },
			{ ColorFormat::_enum::R_64F, vk::Format::eR64Sfloat },
			{ ColorFormat::_enum::Rint_8, vk::Format::eR8Uint },
			{ ColorFormat::_enum::Rint_16, vk::Format::eR16Uint },
			{ ColorFormat::_enum::Rint_32, vk::Format::eR32Uint },
			{ ColorFormat::_enum::Rint_64, vk::Format::eR64Uint },
			{ ColorFormat::_enum::RG_8, vk::Format::eR8G8Unorm },
			{ ColorFormat::_enum::RGF_16, vk::Format::eR16G16Sfloat },
			{ ColorFormat::_enum::RGB_8, vk::Format::eR8G8B8Unorm },
			{ ColorFormat::_enum::RGBF_16, vk::Format::eR16G16B16Sfloat },
			{ ColorFormat::_enum::RGBF_32, vk::Format::eR32G32B32Sfloat },
			{ ColorFormat::_enum::RGBA_8, vk::Format::eR8G8B8A8Unorm },
			{ ColorFormat::_enum::RGBAF_16, vk::Format::eR16G16B16A16Sfloat },
			{ ColorFormat::_enum::RGBAF_32, vk::Format::eR32G32B32A32Sfloat },
			{ ColorFormat::_enum::BGRA_8,   vk::Format::eB8G8R8A8Unorm },
			{ ColorFormat::_enum::SRGB ,   vk::Format::eR8G8B8Srgb },
			{ ColorFormat::_enum::SRGBA,   vk::Format::eR8G8B8A8Srgb },
			{ ColorFormat::_enum::DXT1,vk::Format::eBc1RgbUnormBlock },
			{ ColorFormat::_enum::DXT3,vk::Format::eBc2UnormBlock },
			{ ColorFormat::_enum::DXT5,vk::Format::eBc3UnormBlock },
			{ ColorFormat::_enum::DXT1_A,vk::Format::eBc1RgbaUnormBlock },
			{ ColorFormat::_enum::SRGB_DXT1,vk::Format::eBc1RgbSrgbBlock },
			{ ColorFormat::_enum::SRGB_DXT3,vk::Format::eBc2SrgbBlock },
			{ ColorFormat::_enum::SRGB_DXT5,vk::Format::eBc3SrgbBlock },
			{ ColorFormat::_enum::SRGBA_DXT1,vk::Format::eBc1RgbaSrgbBlock },
		};

	}
	hash_table<vk::Format, vk::Format> UnSrgbMap()
	{
		return hash_table<vk::Format, vk::Format>
		{
			{ vk::Format::eR8G8B8Srgb, vk::Format::eR8G8B8Unorm       },
			{ vk::Format::eR8G8B8A8Srgb     ,vk::Format::eR8G8B8A8Unorm },
			{ vk::Format::eBc1RgbSrgbBlock  ,vk::Format::eBc1RgbUnormBlock },
			{ vk::Format::eBc2SrgbBlock     ,vk::Format::eBc2UnormBlock },
			{ vk::Format::eBc3SrgbBlock     ,vk::Format::eBc3UnormBlock },
			{ vk::Format::eBc1RgbaSrgbBlock ,vk::Format::eBc1RgbaUnormBlock },
		};

	}

	hash_table<CompareOp, vk::CompareOp> CompareOpMap()
	{
		return hash_table<CompareOp, vk::CompareOp>
		{
			{CompareOp::eNever,			  vk::CompareOp::eNever          },
			{CompareOp::eLess,			  vk::CompareOp::eLess           },
			{CompareOp::eEqual ,		  vk::CompareOp::eEqual          },
			{CompareOp::eLessOrEqual ,	  vk::CompareOp::eLessOrEqual    },
			{CompareOp::eGreater ,		  vk::CompareOp::eGreater        },
			{CompareOp::eNotEqual,		  vk::CompareOp::eNotEqual       },
			{CompareOp::eGreaterOrEqual , vk::CompareOp::eGreaterOrEqual },
			{CompareOp::eAlways,          vk::CompareOp::eAlways         },
		};
	}


	vk::Format MapFormat(TextureFormat tf)
	{
		static const auto map = FormatMap();
		return map.find(tf)->second;
	}
	vk::Format MapFormat(ColorFormat tf)
	{
		static const auto map = CFormatMap();
		return map.find(tf)->second;
	}
	TextureFormat MapFormat(vk::Format tf)
	{
		static const auto map = hlp::ReverseMap(FormatMap());
		return map.find(tf)->second;
	}
	vk::Format UnSrgb(vk::Format f)
	{
		static const auto map = UnSrgbMap();
		vk::Format result = f;
		auto itr = map.find(f);
		if (itr != map.end())
			result = itr->second;
		return result;
	}
	vk::Format ToSrgb(vk::Format f)
	{
		static const auto map = hlp::ReverseMap(UnSrgbMap());
		vk::Format result = f;
		auto itr = map.find(f);
		if (itr != map.end())
			result = itr->second;
		return result;
	}

	vk::CompareOp MapCompareOp(CompareOp compare_op)
	{
		vk::CompareOp mode = vk::CompareOp::eNever;
		static const hash_table<CompareOp, vk::CompareOp> map = CompareOpMap();
		auto itr = map.find(compare_op);
		if (itr != map.end())
			mode = itr->second;

		return mode;
	}

	void PrintFormatBlitCompatibility()
	{
		auto map = CFormatMap();
		auto pdevice = View().PDevice();
		for (auto& [ecf, format] : map)
		{
			ColorFormat cf = ecf;
			auto prop = pdevice.getFormatProperties(format);

			std::cout << cf.to_string() << std::endl;
			if (prop.linearTilingFeatures & vk::FormatFeatureFlagBits::eBlitDst)
				std::cout << "\tLinear tiling has eBlitDst\n";
			else
				std::cout << "\tLinear tiling does not have eBlitDst\n";
			if (prop.optimalTilingFeatures & vk::FormatFeatureFlagBits::eBlitDst)
				std::cout << "\toptimal tiling has eBlitDst\n";
			else
				std::cout << "\toptimal tiling does not have eBlitDst\n";
			if (prop.linearTilingFeatures & vk::FormatFeatureFlagBits::eBlitSrc)
				std::cout << "\tLinear tiling has eBlitSrc\n";
			else
				std::cout << "\tLinear tiling does not have eBlitSrc\n";
			if (prop.optimalTilingFeatures & vk::FormatFeatureFlagBits::eBlitSrc)
				std::cout << "\toptimal tiling has eBlitSrc\n";
			else
				std::cout << "\toptimal tiling does not have eBlitSrc\n";
		}
	}
}