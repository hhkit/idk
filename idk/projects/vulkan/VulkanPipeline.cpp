#include "pch.h"
#include "VulkanPipeline.h"
#include <vgfx_converters.h>
namespace idk
{
	vector<vk::VertexInputAttributeDescription> VulkGfxPipeline::GetVtxAttribInfo(const config_t& config) const
	{
		[[maybe_unused]] auto [binding,attrib] =vgfx::ConvertVtxDesc(config.buffer_descriptions);

		return attrib;
	}

	vector<vk::VertexInputBindingDescription> VulkGfxPipeline::GetVtxBindingInfo(const config_t& config) const
	{
		[[maybe_unused]] auto [binding, attrib] = vgfx::ConvertVtxDesc(config.buffer_descriptions);
		return binding;
	}
}
