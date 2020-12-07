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

#include <vkn/BufferNBuilder.h>

#include <ds/lazy_vector.h>


#include <util/PoolContainer.h>

#include "UniformUtils.h"

namespace idk::vkn
{

	class ShaderModule;
	struct UboData {};
	struct TexData {};


	struct UniformManager : public UniformUtils
	{
		using set_bindings = binding_manager::set_bindings;

		void SetUboManager(UboManager& ubo_manager) noexcept;

		//Helper function to call the relevant registration functions
		void AddShader(const ShaderModule& module);
		//Helper function to call the relevant deregistration functions
		void RemoveShader(const ShaderModule& module);

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
		void GenerateDescriptorSets(span<const set_binding_t> bindings, DescriptorUpdateData& dud,DescriptorsManager& dm, vector<vk::DescriptorSet>& descriptor_managers);
		void Reset()
		{
			_uniform_names.clear();
			_bindings.Reset();
			_collated_layouts.clear();
			_binding_info_builder.buffer.clear();
			//_dud.Reset();
			_dbg.Reset();
		}
	private:
		PooledContainer<hash_table<string, UniInfo>> _uniform_names;
		UboManager* _ubo_manager;
		binding_manager _bindings;
		CollatedLayouts_t _collated_layouts;
		BufferNBuilder<BindingInfo> _binding_info_builder;
		//vector<BindingInfo> _buffer; //TODO: Move protection
		//vector_span_builder<BindingInfo> _buffer_builder{ _buffer };
		//DescriptorUpdateData _dud;
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
			void MarkSet(uint32_t set);

			bool Validate(const binding_manager& _bindings)const;

			void Reset();
		};
		DebugInfo _dbg;
	};
}