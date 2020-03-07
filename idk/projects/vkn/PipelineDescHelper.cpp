#include "pch.h"
#include "PipelineDescHelper.h"
#include <vkn/ShaderModule.h>
#include <res/ResourceHandle.inl>
//Store first

namespace idk::vkn
{
	void PipelineDescHelper::StoreBufferDescOverrides(const pipeline_config& config)
	{
		buffer_desc_overrides = config.buffer_descriptions;
		override_attr_mapping.clear();
		size_t i = 0;
		for (auto& desc : buffer_desc_overrides)
		{
			for (auto& attr : desc.attributes)
			{
				override_attr_mapping[attr.location] = i;
			}
			++i;
		}
	}

	//Does not help you store your overrides.
	//Does not help you store your overrides.

	void PipelineDescHelper::UseShaderAttribs(const vector<RscHandle<ShaderProgram>>& shader_handles, pipeline_config& config)
	{
		config.buffer_descriptions.clear();
		hash_set<uint32_t> layouts;
		layouts.reserve(32);
		for (auto& module : shader_handles)
		{
			auto& mod = module.as<ShaderModule>();
			if (!mod.HasCurrent())
				continue;
			//if (mod.NeedUpdate()) //Excluded. leave it to pipeline manager's check for update.
			//	mod.UpdateCurrent(fo_index);
			auto& desc = mod.AttribDescriptions();
			for (auto& desc_set : desc)
			{
				bool skip = false;
				for (auto attrib : desc_set.attributes)
				{
					if (attrib.fixed_location)
					{
						if (layouts.find(attrib.location) != layouts.end())
						{
							skip = true;
							break;
						}
						layouts.emplace(attrib.location);
					}
				}
				if (!skip)
					config.buffer_descriptions.emplace_back(desc_set);
			}
			//shaders.emplace_back(mod.Stage(), mod.Module());
			if (mod.Stage() == vk::ShaderStageFlagBits::eFragment)
				config.frag_shader = module;

			if (mod.Stage() == vk::ShaderStageFlagBits::eVertex)
				config.vert_shader = module;
			ApplyBufferDescOverrides(config);
		}
	}

	void PipelineDescHelper::ApplyBufferDescOverrides(pipeline_config& config)
	{
		vector<size_t> binding_removal_indices;
		auto& config_bdesc = config.buffer_descriptions;
		size_t i = 0;
		for (auto& cbd : config_bdesc)
		{
			for (auto& attr : cbd.attributes)
			{
				auto itr = override_attr_mapping.find(attr.location);
				if (itr != override_attr_mapping.end())
				{
					binding_removal_indices.emplace_back(i);
				}
			}
			++i;
		}
		std::sort(binding_removal_indices.begin(), binding_removal_indices.end());
		{
			auto end_itr = std::unique(binding_removal_indices.begin(), binding_removal_indices.end());
			binding_removal_indices.resize(end_itr - binding_removal_indices.begin());
		}
		//erase from behind to avoid invalidating indices
		for (auto itr = binding_removal_indices.rbegin(); itr < binding_removal_indices.rend(); ++itr)
		{
			auto index = *itr;
			config_bdesc.erase(config_bdesc.begin() + index);
		}
		config_bdesc.insert(config_bdesc.end(), buffer_desc_overrides.begin(), buffer_desc_overrides.end());

	}

}