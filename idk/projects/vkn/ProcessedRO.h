#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/utils/utils.h>
#include <gfx/pipeline_config.h>
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
			std::optional<vk::Buffer> GetBuffer()const;
			std::optional<image_t> GetImage()const
			{
				using Type = image_t;
				std::optional<Type> ret;
				if (ubuffer.index() == meta::IndexOf<data_t, Type>::value)
					ret = std::get<Type>(ubuffer);
				return ret;
			}
			vk::DescriptorSetLayout GetLayout()const
			{
				return layout;
			}
		};
		const RenderObject& Object()const
		{
			return *itr;
		}

		//set, update_instr
		const RenderObject* itr;
		hash_table<uint32_t, vector<BindingInfo>> bindings;
		shared_ptr<pipeline_config> config;

		std::optional<RscHandle<ShaderProgram>> vertex_shader;
		std::optional<RscHandle<ShaderProgram>> geom_shader;
		std::optional<RscHandle<ShaderProgram>> frag_shader;
	};
}