#pragma once
#include <idk.h>
#include <vkn/VulkanPipeline.h>

namespace idk::vkn 
{
	struct draw_call
	{
		struct Binding
		{
			uint32_t first_binding {};
			uint32_t offset        {};
			Binding() = default;
			Binding(uint32_t fb, uint32_t o) : first_binding{ fb }, offset{ o }{}
		};

		vector<Binding>  vtx_binding{};
		//probably something for index too
		vector<Binding>  idx_binding{};
		//Probably something for UBO too
		uint32_t         instance_count{};
		uint32_t         vertex_count  {};
		VulkanPipeline* pipeline      {};
		uniform_info     uniform_info;
	};
}