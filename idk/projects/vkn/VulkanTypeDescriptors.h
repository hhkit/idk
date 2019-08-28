#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <gfx/vertex_descriptor.h>

namespace idk::vkn
{
	struct VulkanType
	{
		vk::Format type{};
		uint32_t   size{};
	};

	const static inline hash_table<vtx::Attrib, VulkanType> VulkanAttribs
	{
		std::make_pair(vtx::Attrib::Position,   VulkanType{ vk::Format::eR32G32B32Sfloat   , 3}),
		std::make_pair(vtx::Attrib::Normal,     VulkanType{ vk::Format::eR32G32B32Sfloat   , 3}),
		std::make_pair(vtx::Attrib::Tangent,    VulkanType{ vk::Format::eR32G32B32Sfloat   , 3}),
		std::make_pair(vtx::Attrib::UV,         VulkanType{ vk::Format::eR32G32Sfloat      , 2}),
		std::make_pair(vtx::Attrib::BoneID,     VulkanType{ vk::Format::eR32G32B32A32Sfloat, 4}),
		std::make_pair(vtx::Attrib::BoneWeight, VulkanType{ vk::Format::eR32G32B32A32Sfloat, 4}),
	};

	struct VulkanDescriptor
	{
		vtx::Attrib attrib = vtx::Attrib::Position;
		uint32_t    offset = 0;
		uint32_t    stride = 0;
	};
}