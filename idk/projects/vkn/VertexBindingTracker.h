#pragma once
#include <idk.h>
#include <gfx/ShaderProgram.h>
#include <gfx/pipeline_config.h>
#include <vkn/PipelineDescHelper.h>
#include <vkn/FixedArenaAllocator.h>
namespace idk::vkn
{
	template<typename K,typename T>
	using hash_table_t = std::unordered_map<K, T, std::hash<K>, std::equal_to<K>, FixedArenaAllocator<std::pair<const K, T>>>;
	struct VertexBindingTracker
	{
		void Update(RscHandle<ShaderProgram>  vtx_shader);
		void Update(const pipeline_config& config, RscHandle<ShaderProgram>  vtx_shader);
		std::optional<uint32_t> GetBinding(uint32_t location)const;
		const vector<buffer_desc>& GetDescriptors()const;
		hash_table_t<uint32_t,uint32_t> loc2bind;
		void Reset()
		{
			loc2bind.clear();
			pipeline_helper.Reset();
			_config = {};
		}
	private:
		void UpdateLoc();
		PipelineDescHelper pipeline_helper;
		pipeline_config _config;
	};
}