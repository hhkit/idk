#include "pch.h"
#include "GfxConverters.h"
#undef max
#undef min

namespace idk::vkn::hlp
{
	vk::VertexInputRate MapVtxRate(VertexRate rate)
	{
		static const idk::hash_table<VertexRate, vk::VertexInputRate> map
		{
			{ VertexRate::eVertex,vk::VertexInputRate::eVertex }
			,{ VertexRate::eInstance,vk::VertexInputRate::eInstance }
		};
		return map.find(rate)->second;
	}

	vk::Format MapVtxFormat(AttribFormat rate)
	{
		static const idk::hash_table<AttribFormat, vk::Format> map
		{
			{ AttribFormat::eSVec2,vk::Format::eR32G32Sfloat }
			,{ AttribFormat::eSVec3,vk::Format::eR32G32B32Sfloat }
			,{ AttribFormat::eSVec4,vk::Format::eR32G32B32A32Sfloat }
		};
		auto itr = map.find(rate);
		assert(itr != map.end());
		return itr->second;
	}

	vk::VertexInputBindingDescription ConvertVtxBinding(const buffer_desc::binding_info& binding)
	{
		return vk::VertexInputBindingDescription
		{
			*binding.binding_index
			,binding.stride
			,MapVtxRate(binding.vertex_rate)
		};
	}

	vk::VertexInputAttributeDescription ConvertVtxAttrib(const buffer_desc::attribute_info& attrib, uint32_t binding_index)
	{
		return vk::VertexInputAttributeDescription
		{
			attrib.location
			,binding_index
			,MapVtxFormat(attrib.format)
			,attrib.offset
		};
	}

	std::pair<idk::vector<vk::VertexInputBindingDescription>, idk::vector<vk::VertexInputAttributeDescription>> ConvertVtxDesc(const vector<buffer_desc>& descs)
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
		return result;
	}
}