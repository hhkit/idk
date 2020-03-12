#pragma once
#include <vkn/VulkanHashes.h>
#include <vkn/DescriptorCountArray.h>
#include <ds/result.inl>
#include <ds/lazy_vector.h>
#include <vkn/VknTextureView.h>
#include <vkn/UboManager.inl>
#include <vkn/vector_span.h>
#include <vkn/vector_span_builder.h>
#include <vkn/DescriptorsManager.h>
#include <vkn/DescriptorUpdateData.h>

#include <ds/lazy_vector.h>

namespace idk::vkn
{

	struct UboData {};
	struct TexData {};

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
				hash_table<uint32_t, vector<std::optional<BindingInfo>>>  bindings;
				std::optional<vk::DescriptorSet> ds_override{};
				DsCountArray total_desc;
				bool dirty = false;

				vector<BindingInfo> scratch_out;

				void SetLayout(vk::DescriptorSetLayout new_layout, const DsCountArray& total_descriptors, bool clear_bindings = false);
				bool SetOverride(vk::DescriptorSet ds,vk::DescriptorSetLayout layout);
				void Bind(BindingInfo info);
				void Unbind(uint32_t binding);
				monadic::result< binding_span, string> FinalizeDC(CollatedLayouts_t& collated_layouts, vector_span_builder<BindingInfo>& builder);
			};
			using binding_span = binding_manager::binding_span;
			hash_table<set_t, set_bindings> curr_bindings;

			std::optional<vk::DescriptorSetLayout> GetLayout(set_t);

			bool SetOverride(set_t set, vk::DescriptorSet ds, vk::DescriptorSetLayout dsl);

			void AddBinding(set_t set, vk::DescriptorSetLayout layout, const DsCountArray& counts);
			void RemoveBinding(set_t set);
			void MarkDirty();

			bool BindUniformBuffer(UniInfo info, uint32_t array_index, vk::Buffer buffer,size_t offset, size_t size, bool skip_if_bound = false);
			bool BindSampler(UniInfo info, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound = false, vk::ImageLayout layout = vk::ImageLayout::eGeneral);
			bool BindAttachment(UniInfo info, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound = false, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);

			bool is_bound(set_t set, uint32_t binding_index, uint32_t array_index)const noexcept;
		};
	};

	struct UniformManager : public UniformUtils
	{
		using set_bindings = binding_manager::set_bindings;

		void SetUboManager(UboManager& ubo_manager) noexcept;

		//Do this first
		void AddBinding(binding_manager::set_t set, vk::DescriptorSetLayout layout, const DsCountArray& counts);
		//Before this
		bool RegisterUniforms(string name, binding_manager::set_t set, uint32_t binding, uint32_t size);
		void RemoveBinding(binding_manager::set_t set);

		bool BindDescriptorSet(uint32_t set, vk::DescriptorSet ds, vk::DescriptorSetLayout dsl);
		bool BindUniformBuffer(string_view uniform_name, uint32_t array_index, string_view data, bool skip_if_bound = false);
		bool BindSampler(string_view uniform_name, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound = false, vk::ImageLayout layout = vk::ImageLayout::eGeneral);
		bool BindAttachment(string_view uniform_name, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound = false, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);

		bool BindUniformBuffer(const UniInfo& info, uint32_t array_index, string_view data, bool skip_if_bound = false);
		bool BindSampler(const UniInfo& info, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound = false, vk::ImageLayout layout = vk::ImageLayout::eGeneral);
		bool BindAttachment(const UniInfo& info, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound = false, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);

		//set, bindings
		using set_binding_t = std::pair<uint32_t, binding_manager::binding_span>;
		std::optional<vector_span<set_binding_t>> FinalizeCurrent(vector<set_binding_t>& all_sets);
		void GenerateDescriptorSets(span<const set_binding_t> bindings,DescriptorsManager& dm, vector<vk::DescriptorSet>& descriptor_managers);
	private:
		hash_table<string, UniInfo> _uniform_names;
		UboManager* _ubo_manager;
		binding_manager _bindings;
		CollatedLayouts_t _collated_layouts;
		vector<BindingInfo> _buffer;
		vector_span_builder<BindingInfo> _buffer_builder{ _buffer };
		DescriptorUpdateData _dud;
		struct DebugInfo
		{
			struct Set
			{
				lazy_vector<uint32_t> bindings;
				size_t bound = 0;
			};
			lazy_vector<Set> sets;
			void RegisterRequiredBinding(uint32_t set, uint32_t binding);
			void RemoveRequiredSet(uint32_t set);
			void MarkBinding(uint32_t set, uint32_t binding);

			bool Validate(const binding_manager& _bindings)const;
		};
		DebugInfo _dbg;
	};
}