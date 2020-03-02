#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <gfx/pipeline_config.h>

namespace idk::vkn
{
	struct PipelineDescHelper
	{

		vector<buffer_desc> buffer_desc_overrides;
		hash_table<uint32_t, size_t> override_attr_mapping;

		//Store first
		void StoreBufferDescOverrides(const pipeline_config& config);
		//Does not help you store your overrides.
		//Does not help you store your overrides.
		void UseShaderAttribs(const vector<RscHandle<ShaderProgram>>& shader_handles, pipeline_config& config);
	private:
		void ApplyBufferDescOverrides(pipeline_config& config);
	};
}