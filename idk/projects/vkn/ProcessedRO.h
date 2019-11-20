#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/utils/utils.h>
#include <gfx/pipeline_config.h>
#include <meta/stl_hack.h>

namespace idk
{
	struct RenderObject;
}
namespace idk::vkn
{

	struct ProcessedRO
	{
		struct ImageBinding
		{
			vk::ImageView view;
			vk::Sampler sampler;
			vk::ImageLayout layout;
		};
		using image_t = ImageBinding;
		struct BindingInfo
		{
			using data_t =variant<vk::Buffer, image_t>;
			uint32_t binding;
			data_t ubuffer;
			uint32_t buffer_offset;
			uint32_t arr_index;
			size_t size;
			vk::DescriptorSetLayout layout;

			BindingInfo(
				uint32_t binding_,
				vk::Buffer& ubuffer_,
				uint32_t buffer_offset_,
				uint32_t arr_index_,
				size_t size_,
				vk::DescriptorSetLayout layout_
			) :
				binding{ binding_ },
				ubuffer{ ubuffer_ },
				buffer_offset{ buffer_offset_ },
				arr_index{ arr_index_ },
				size{ size_ },
				layout{ layout_ }
			{
			}
			BindingInfo(
				uint32_t binding_,
				image_t ubuffer_,
				uint32_t buffer_offset_,
				uint32_t arr_index_,
				size_t size_,
				vk::DescriptorSetLayout layout_
			) :
				binding{ binding_ },
				ubuffer{ ubuffer_ },
				buffer_offset{ buffer_offset_ },
				arr_index{ arr_index_ },
				size{ size_ },
				layout{ layout_ }
			{
			}
			BindingInfo() = default;
			std::optional<vk::Buffer> GetBuffer()const;
			std::optional<image_t> GetImage()const;
			bool IsImage()const;
			vk::DescriptorSetLayout GetLayout()const;
		};
		//set, update_instr
		const RenderObject* itr=nullptr;
		hash_table<uint32_t, vector<BindingInfo>> bindings; //Deprecate this
		shared_ptr<pipeline_config> config;

		std::optional<RscHandle<ShaderProgram>> vertex_shader;
		std::optional<RscHandle<ShaderProgram>> geom_shader;
		std::optional<RscHandle<ShaderProgram>> frag_shader;

		//set, ds
		vector<std::optional<vk::DescriptorSet>> descriptor_sets = vector<std::optional<vk::DescriptorSet>>( 8 );
		bool rebind_shaders = false;
		size_t num_instances = 1;
		size_t inst_offset   = 0;
		/*
		ProcessedRO(
			//set, update_instr
			const RenderObject* itr_                                 = nullptr,
			hash_table<uint32_t, vector<BindingInfo>> bindings_      = {}, //Deprecate this
			shared_ptr<pipeline_config> config_                      = {},

			std::optional<RscHandle<ShaderProgram>> vertex_shader_   = {},
			std::optional<RscHandle<ShaderProgram>> geom_shader_     = {},
			std::optional<RscHandle<ShaderProgram>> frag_shader_     = {},

			//set, ds
			vector<std::optional<vk::DescriptorSet>> descriptor_sets = vector<std::optional<vk::DescriptorSet>>(8),
			bool rebind_shaders_                                     = false,
			size_t num_instances_                                    = 1,
			size_t inst_offset_                                      = 0,
		) = default;

		*/
		//ProcessedRO() = default;
		//ProcessedRO(ProcessedRO&&)noexcept = default;
		//ProcessedRO& operator=(ProcessedRO&&)noexcept = default;
		//ProcessedRO(const ProcessedRO&) = default;
		//ProcessedRO& operator=(const ProcessedRO&) = default;

		const RenderObject& Object()const
		{
			return *itr;
		}
		const std::optional<vk::DescriptorSet>& GetDescriptorSet(uint32_t set)const;
		void SetDescriptorSet(uint32_t set, vk::DescriptorSet ds);
	};
}
MARK_NON_COPY_CTORABLE(idk::vkn::ProcessedRO)