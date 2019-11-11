#pragma once
#include <idk.h>
#include <gfx/buffer_desc.h>
#include <vulkan/vulkan.hpp>
#include <idk/gfx/pipeline_config.h>   //PrimitiveTopology

namespace idk::vkn::hlp
{
	using VertexInputDescriptions = std::pair < vector<vk::VertexInputBindingDescription>, vector<vk::VertexInputAttributeDescription >>;

	auto MapVtxRate       (VertexRate   rate         ) -> vk::VertexInputRate;
	auto MapVtxFormat     (AttribFormat rate         ) -> vk::Format;
	auto MapPrimTopology  (PrimitiveTopology topology) -> vk::PrimitiveTopology;
	auto ConvertVtxBinding(const buffer_desc::binding_info& binding ) -> vk::VertexInputBindingDescription;
	auto ConvertVtxAttrib (const buffer_desc::attribute_info& attrib, uint32_t binding_index)->vk::VertexInputAttributeDescription;

	auto ConvertVtxDesc   (const vector<buffer_desc>& descs) -> VertexInputDescriptions;

	auto MapStage (uniform_layout_t::UniformStage                        stage ) ->vk::ShaderStageFlagBits;
	auto MapStages(const decltype(uniform_layout_t::bindings_t::stages)& stages) ->vk::ShaderStageFlags;
}