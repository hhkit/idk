#include "pch.h"
#include "vgfx_converters.h"
#undef max
#undef min
namespace vgfx
{

	vk::VertexInputRate vgfx::MapVtxRate(VertexRate rate)
	{
		static const idk::hash_table<VertexRate, vk::VertexInputRate> map
		{
			{ VertexRate::eVertex,vk::VertexInputRate::eVertex }
			,{ VertexRate::eInstance,vk::VertexInputRate::eInstance }
		};
		return map.find(rate)->second;
	}

	vk::Format vgfx::MapVtxFormat(AttribFormat rate)
	{
		static const idk::hash_table<AttribFormat, vk::Format> map
		{
			{ AttribFormat::eSVec2,vk::Format::eR32G32Sfloat }
			,{ AttribFormat::eSVec3,vk::Format::eR32G32B32Sfloat }
			,{ AttribFormat::eSVec4,vk::Format::eR32G32B32A32Sfloat }
		};
		return map.find(rate)->second;
	}

	vk::VertexInputBindingDescription vgfx::ConvertVtxBinding(const buffer_desc::binding_info& binding)
	{
		return vk::VertexInputBindingDescription
		{
			*binding.binding_index
			,binding.stride
			,MapVtxRate(binding.vertex_rate)
		};
	}

	std::pair<idk::vector<vk::VertexInputBindingDescription>, idk::vector<vk::VertexInputAttributeDescription>> vgfx::ConvertVtxDesc(const vector<buffer_desc>& descs)
	{
		std::pair < idk::vector<vk::VertexInputBindingDescription>, idk::vector<vk::VertexInputAttributeDescription >>
			result;
		uint32_t loc = 0;
		uint32_t binding = 0;
		bool optional_was_set = false;
		for (auto& pdesc : descs)
		{
			auto desc = pdesc.Process(loc);
			auto actual_binding = binding;
			if (!desc.binding.binding_index)
			{
				if (optional_was_set)
					throw std::runtime_error("ConvertVtxDesc: descriptors with unset locations were queued after descriptors with set locations.");
				desc.binding.binding_index = binding;
			}
			else
			{
				optional_was_set = true;
				actual_binding = *desc.binding.binding_index;
			}
			result.first.emplace_back(ConvertVtxBinding(desc.binding));
			for (auto& attrib : desc.attributes)
			{
				result.second.emplace_back(ConvertVtxAttrib(attrib, actual_binding));
				loc = std::max(loc, attrib.location);
			}
			++binding;
			++loc;
		}
	}

}