#pragma once
#include <idk.h>
#include <vkn/VulkanPipeline.h>
#include <meta/meta.h>
namespace idk::vkn 
{
	struct draw_call
	{
		struct uniform_binding
		{
			uint32_t set{};
			uint32_t binding{};
			string   data   {}; //Probably can optimize this to not require dynamic allocation by using a fixed buffer.
			template<typename T, typename = std::enable_if_t<!is_container_v<T>>>
			uniform_binding(uint32_t st, uint32_t bnd, const T& val)
				:binding{ bnd }, set{ st }, data{r_cast<const char*>(&val),sizeof(val)}
			{

			}
		};
		struct Binding
		{
			uint32_t first_binding {};
			uint32_t offset        {};
			Binding() = default;
			Binding(uint32_t fb, uint32_t o) : first_binding{ fb }, offset{ o }{}
		};

		vector<uniform_binding> uniforms      {};
		vector<Binding>         vtx_binding   {};
		//probably something for index too
		vector<Binding>         idx_binding   {};
		uint32_t                instance_count{};
		uint32_t                vertex_count  {};
		VulkanPipeline*         pipeline      {};


		void SetBinding (const uniform_binding& uniform);
		void SetBindings(const vector<uniform_binding>& uniform);
		void SetBinding (uniform_binding&& uniform);
		void SetBindings(vector<uniform_binding>&& uniform);
	};
}