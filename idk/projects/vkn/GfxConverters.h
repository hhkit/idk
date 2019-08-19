#pragma once
#include <idk.h>
#include <gfx/buffer_desc.h>
#include <vulkan/vulkan.hpp>

namespace idk::vkn::hlp
{
	auto MapVtxRate       (VertexRate rate) -> vk::VertexInputRate;
	auto MapVtxFormat     (AttribFormat rate) -> vk::Format;
	auto ConvertVtxBinding(const buffer_desc::binding_info& binding) -> vk::VertexInputBindingDescription;
	auto ConvertVtxAttrib (const buffer_desc::attribute_info& attrib, uint32_t binding_index)->vk::VertexInputAttributeDescription;

	using VertexInputDescriptions = std::pair < vector<vk::VertexInputBindingDescription>, vector<vk::VertexInputAttributeDescription >>;
	auto ConvertVtxDesc   (const vector<buffer_desc>& descs) -> VertexInputDescriptions;
}