#pragma once
#include <vulkan/vulkan.hpp>
#include <idk.h>

namespace idk::vkn::hlp
{
	inline vk::Extent2D ToExtent2D(uvec2 size)noexcept { return vk::Extent2D{ size.x,size.y }; }
	inline vk::Offset2D ToOffset2D(ivec2 offset)noexcept { return vk::Offset2D{ offset.x, offset.y }; }
	inline vk::Rect2D ToRect2D(ivec2 offset, uvec2 sz)noexcept { return vk::Rect2D{ ToOffset2D(ivec2{offset}),ToExtent2D(sz) }; }
	inline vk::Rect2D ToRect2D(rect r)noexcept { return vk::Rect2D{ ToOffset2D(ivec2{r.position}),ToExtent2D(uvec2{r.size}) }; }
	inline vk::Rect2D ToRect2D(ivec2 offset, ivec2 sz)noexcept { return ToRect2D(offset, uvec2{ sz }); }

	inline vk::Extent3D ToExtent3D(uvec3 size  )noexcept { return vk::Extent3D{ (size.x), (size.y), (size.z) };  }
	inline vk::Offset3D ToOffset3D(ivec3 offset)noexcept { return vk::Offset3D{ offset.x,offset.y,offset.z };
	}
}