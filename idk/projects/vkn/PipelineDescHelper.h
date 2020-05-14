#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <gfx/pipeline_config.h>

#include <vkn/FixedArenaAllocator.h>
namespace idk::vkn
{
	//template<typename K, typename T>
	//using hash_table_t = std::unordered_map<K, T, std::hash<K>, std::equal_to<K>, FixedArenaAllocator<std::pair<const K, T>>>;
	struct PipelineDescHelper
	{

		vector<buffer_desc> buffer_desc_overrides;
		hash_table<uint32_t, size_t> override_attr_mapping;

		//Store first
		void StoreBufferDescOverrides(const pipeline_config& config);
		//Does not help you store your overrides.
		//Does not help you store your overrides.
		void UseShaderAttribs(const vector<RscHandle<ShaderProgram>>& shader_handles, pipeline_config& config);

		void Reset()
		{
			buffer_desc_overrides.clear();
			override_attr_mapping.clear();
		}
	private:
		void ApplyBufferDescOverrides(pipeline_config& config);
	};
}