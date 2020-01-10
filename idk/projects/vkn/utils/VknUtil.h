#pragma once
#include <vulkan/vulkan.hpp>
#include <idk.h>

namespace idk::vkn::hlp
{
	inline vk::Extent2D ToExtent2D(ivec2 size)noexcept { return vk::Extent2D{ static_cast<uint32_t>(size.x),static_cast<uint32_t>(size.y) }; }
	inline vk::Offset2D ToOffset2D(ivec2 offset)noexcept { return vk::Offset2D{ offset.x, offset.y }; }
	inline vk::Rect2D ToRect2D(ivec2 offset, ivec2 sz)noexcept { return vk::Rect2D{ ToOffset2D(ivec2{offset}),ToExtent2D(sz) }; }

	inline vk::Extent3D ToExtent3D(ivec3 size  )noexcept { return vk::Extent3D{ static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), static_cast<uint32_t>(size.z) };  }
	inline vk::Offset3D ToOffset3D(ivec3 offset)noexcept { return vk::Offset3D{ offset.x,offset.y,offset.z };
	}
}