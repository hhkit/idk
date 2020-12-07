#pragma once
#include "VulkanView.h"
#include <vkn/DescriptorCountArray.h>
#include <util/PoolContainer.h>
#include <vkn/vector_span.h>
#include <ds/result.inl>
#include <vkn/VknTextureView.h>
#include <vkn/vector_span_builder.h>

namespace idk::vkn
{

	using desc_type_info = DescriptorTypeI; // pair<num_ds,num_descriptors_per_type>
	using CollatedLayouts_t = hash_table < vk::DescriptorSetLayout, std::pair<uint32_t, DsCountArray>>;
	template<vk::DescriptorType type>
	static constexpr size_t desc_type_idx = desc_type_info::map<type>();
	struct UniformUtils
	{
		struct ImageBinding
		{
			vk::ImageView view;
			vk::Sampler sampler;
			vk::ImageLayout layout;
		};
		struct AttachmentBinding
		{
			vk::ImageView view;
			vk::ImageLayout layout;
		};
		struct BufferBinding
		{
			vk::Buffer buffer;
			size_t offset;
		};
		using image_t = ImageBinding;
		struct BindingInfo
		{
			using data_t =variant<BufferBinding, image_t, AttachmentBinding>;
			uint32_t binding;
			data_t ubuffer;
			uint32_t arr_index;
			size_t size;
			vk::DescriptorSetLayout layout;
			size_t type_index = 0;

			BindingInfo(
				uint32_t binding_,
				vk::Buffer ubuffer_,
				size_t buffer_offset_,
				uint32_t arr_index_,
				size_t size_,
				vk::DescriptorSetLayout layout_
			);
			BindingInfo(
				uint32_t binding_,
				image_t ubuffer_,
				uint32_t arr_index_,
				size_t size_,
				vk::DescriptorSetLayout layout_
			);
			BindingInfo(
				uint32_t binding_,
				AttachmentBinding ubuffer_,
				uint32_t arr_index_,
				size_t size_,
				vk::DescriptorSetLayout layout_
			);
			BindingInfo() = default;
			std::optional<BufferBinding> GetBuffer()const;
			std::optional<image_t> GetImage()const;
			std::optional<AttachmentBinding> GetAttachment()const;
			bool IsImage()const;
			bool IsAttachment()const;
			vk::DescriptorSetLayout GetLayout()const;
		};

		struct UniInfo
		{
			uint32_t set;
			uint32_t binding;
			uint32_t size;
			vk::DescriptorSetLayout layout;
		};
		struct binding_manager
		{
			using set_t = uint32_t;
			using binding_t = uint32_t;

			using binding_span = std::variant< vector_span<BindingInfo>, vk::DescriptorSet>;

			struct set_bindings
			{
				vk::DescriptorSetLayout layout;
				PooledContainer<hash_table<uint32_t, PooledContainer<vector<std::optional<BindingInfo>>>>>  bindings;
				std::optional<vk::DescriptorSet> ds_override{};
				DsCountArray total_desc;
				bool dirty = false;

				vector<BindingInfo> scratch_out;

				void SetLayout(vk::DescriptorSetLayout new_layout, const DsCountArray& total_descriptors, bool clear_bindings = false);
				bool SetOverride(vk::DescriptorSet ds, vk::DescriptorSetLayout layout);
				void Bind(BindingInfo info);
				void Unbind(uint32_t binding);
				monadic::result< binding_span, string> FinalizeDC(CollatedLayouts_t& collated_layouts, vector_span_builder<BindingInfo>& builder);
			};
			using binding_span = binding_manager::binding_span;
			PooledContainer<hash_table<set_t, set_bindings>> curr_bindings;

			std::optional<vk::DescriptorSetLayout> GetLayout(set_t);

			bool SetOverride(set_t set, vk::DescriptorSet ds, vk::DescriptorSetLayout dsl);

			void AddBinding(set_t set, vk::DescriptorSetLayout layout, const DsCountArray& counts);
			void RemoveBinding(set_t set);
			void MarkDirty();

			bool BindUniformBuffer(UniInfo info, uint32_t array_index, vk::Buffer buffer, size_t offset, size_t size, bool skip_if_bound = false);
			bool BindSampler(UniInfo info, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound = false, vk::ImageLayout layout = vk::ImageLayout::eGeneral);
			bool BindAttachment(UniInfo info, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound = false, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);

			bool is_bound(set_t set, uint32_t binding_index, uint32_t array_index)const noexcept;

			void Reset()
			{
				curr_bindings.clear();
			}
		};
	};
}