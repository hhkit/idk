#pragma once
#include <idk.h>
#include <gfx/ShaderProgram.h>
#include <gfx/pipeline_config.h>
#include <vkn/PipelineDescHelper.h>
namespace idk::vkn
{
	struct VertexBindingTracker
	{
		void Update(RscHandle<ShaderProgram>  vtx_shader);
		void Update(const pipeline_config& config, RscHandle<ShaderProgram>  vtx_shader);
		std::optional<uint32_t> GetBinding(uint32_t location)const;
		const vector<buffer_desc>& GetDescriptors()const;
		hash_table<uint32_t, uint32_t> loc2bind;
	private:
		void UpdateLoc();
		PipelineDescHelper pipeline_helper;
		pipeline_config _config;
	};
}