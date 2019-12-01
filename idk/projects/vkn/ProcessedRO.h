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
	struct BoundVertexBuffer
	{
		vk::Buffer buffer;
		size_t offset;
	};
	struct BoundIndexBuffer
	{
		vk::Buffer buffer{};
		size_t offset{};
		vk::IndexType index_type{};
	};
	struct ProcessedRO
	{
#pragma region Type declarations
		struct ImageBinding
		{
			vk::ImageView view;
			vk::Sampler sampler;
			vk::ImageLayout layout;
		};
		struct AttachmentBinding
		{
			vk::ImageView view;
			vk::Sampler sampler;
			vk::ImageLayout layout;
		};
		using image_t = ImageBinding;
		struct BindingInfo
		{
			using data_t =variant<vk::Buffer, image_t, AttachmentBinding>;
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
			BindingInfo(
				uint32_t binding_,
				AttachmentBinding ubuffer_,
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
			std::optional<AttachmentBinding> GetAttachment()const;
			bool IsImage()const;
			bool IsAttachment()const;
			vk::DescriptorSetLayout GetLayout()const;
		};
#pragma endregion
		//set, update_instr
		const RenderObject* itr=nullptr;

		//location, vertex buffer
		hash_table<uint32_t, BoundVertexBuffer> attrib_buffers;
		std::optional<BoundIndexBuffer> index_buffer{};
		size_t num_vertices{};
				
		hash_table<uint32_t, vector<BindingInfo>> bindings; //Deprecate this
		shared_ptr<const pipeline_config> config;

		std::optional<RscHandle<ShaderProgram>> vertex_shader;
		std::optional<RscHandle<ShaderProgram>> geom_shader;
		std::optional<RscHandle<ShaderProgram>> frag_shader;

		//set, ds
		vector<std::optional<vk::DescriptorSet>> descriptor_sets = vector<std::optional<vk::DescriptorSet>>( 8 );
		bool rebind_shaders = false;
		size_t num_instances = 1;
		size_t inst_offset   = 0;
		
		const RenderObject& Object()const;
		const std::optional<vk::DescriptorSet>& GetDescriptorSet(uint32_t set)const;
		void SetDescriptorSet(uint32_t set, vk::DescriptorSet ds);
	};
}
MARK_NON_COPY_CTORABLE(idk::vkn::ProcessedRO)