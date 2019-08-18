#pragma once
#include <idk.h>
#include <gfx/buffer_desc.h>
#include <vulkan/vulkan.hpp>

namespace vgfx
{
	using idk::VertexRate;
	using idk::AttribFormat;
	using idk::buffer_desc;
	using idk::vector;
	vk::VertexInputRate MapVtxRate(VertexRate rate);
	vk::Format MapVtxFormat(AttribFormat rate);
	vk::VertexInputBindingDescription ConvertVtxBinding(const buffer_desc::binding_info& binding);
	vk::VertexInputAttributeDescription ConvertVtxAttrib(const  buffer_desc::attribute_info& attrib, uint32_t binding_index);
	std::pair < idk::vector<vk::VertexInputBindingDescription>, idk::vector<vk::VertexInputAttributeDescription >>
		ConvertVtxDesc(const vector<buffer_desc>& descs);

}