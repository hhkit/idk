#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/ProcessedRO.h>
#include <gfx/pipeline_config.h>

#include <vkn/vulkan_enum_info.h>
#include <vkn/UboManager.h>
#include <vkn/ShaderModule.h> //UboInfo
#include <vkn/VulkanHashes.h>
#include <vkn/DescriptorUpdateData.h>
namespace idk::vkn
{
	struct DescriptorUpdateData;
	struct VknCubemap;
	struct DescriptorsManager;
	using desc_type_info = DescriptorTypeI;
	//pair<num_ds,num_descriptors_per_type>
	using CollatedLayouts_t = hash_table < vk::DescriptorSetLayout, std::pair<uint32_t, std::array<uint32_t, DescriptorTypeI::size()>>>;
	template<vk::DescriptorType type>
	static constexpr size_t desc_type_index = desc_type_info::map<type>();

	class PipelineThingy
	{
	public:
		using set_t = uint32_t;
		using binding_t = uint32_t;
		struct set_bindings
		{
			vk::DescriptorSetLayout layout;
			hash_table<uint32_t,vector<std::optional<ProcessedRO::BindingInfo>>>  bindings;
			bool dirty = false;
			void SetLayout(vk::DescriptorSetLayout new_layout, bool clear_bindings = false);
			void Bind(ProcessedRO::BindingInfo info);
			void Unbind(uint32_t binding);
			monadic::result< vector<ProcessedRO::BindingInfo>, string> FinalizeDC(CollatedLayouts_t& collated_layouts);
		};
		
		void SetRef(
			//set, bindings
			UboManager& ubo_manager
		);
		void UnbindShader(ShaderStage stage);
		void BindShader(ShaderStage stage, RscHandle<ShaderProgram> shader);

		std::optional<UboInfo> GetUniform(const string& uniform_name) const;


		template<typename T>
		bool BindUniformBuffer(const string& uniform_name, uint32_t array_index, const T& data, bool skip_if_bound = false);
		bool BindSampler(const string& uniform_name, uint32_t array_index, const VknTexture& texture, bool skip_if_bound = false);
		bool BindSampler(const string& uniform_name, uint32_t array_index, const VknCubemap& texture, bool skip_if_bound = false);

		void FinalizeDrawCall(const RenderObject& ro);
		void FinalizeDrawCall(const RenderObject& ro,size_t num_inst,size_t inst_offset);

		void GenerateDS(DescriptorsManager& d_manager,bool update_ubo_buffers=true);

		void UpdateUboBuffers();

		const vector<ProcessedRO>& DrawCalls()const
		{
			return draw_calls;
		}
		//reserves an extra size chunk
		void reserve(size_t size);

	private:
		DescriptorUpdateData dud{};
		std::optional<RscHandle<ShaderProgram>> shaders[static_cast<size_t>(ShaderStage::Size)];
		vector<ProcessedRO> draw_calls;

		shared_ptr<pipeline_config> prev_config;

		bool shader_changed = false;
		hash_table<set_t, set_bindings> curr_bindings;
		struct Ref
		{
			CollatedLayouts_t collated_layouts;
			UboManager& ubo_manager;
			Ref(
				UboManager* u = nullptr
			)
				:ubo_manager{ *u }
			{}
		};
		Ref ref;
	};

	void UpdateUniformDS(
		vk::Device& device,
		vk::DescriptorSet& dset,
		vector<ProcessedRO::BindingInfo> bindings,
		DescriptorUpdateData& ds_update_data
	);

	template<typename T>
	ProcessedRO::BindingInfo CreateBindingInfo(const UboInfo& obj_uni, uint32_t arr_index, const T& val, UboManager& ubo_manager)
	{
		auto&& [trf_buffer, trf_offset] = ubo_manager.Add(val);
		//collated_bindings[obj_uni.set].emplace_back(
		return ProcessedRO::BindingInfo
		{
			obj_uni.binding,
			trf_buffer,
			trf_offset,
			arr_index,
			obj_uni.size,
			obj_uni.layout
		};
		//);
	}
	namespace detail
	{
		bool is_bound(PipelineThingy::set_bindings& existing_bindings, uint32_t binding_index, uint32_t array_index);
	}
	template<typename T>
	bool PipelineThingy::BindUniformBuffer(const string& uniform_name, uint32_t array_index, const T& data, bool skip_if_bound)
	{
		auto info = GetUniform(uniform_name);
		if (info)
		{
			auto itr = curr_bindings.find(info->set);
			if (!skip_if_bound || itr == curr_bindings.end() || !detail::is_bound(itr->second,info->binding,array_index))
				curr_bindings[info->set].Bind(CreateBindingInfo(*info, array_index, data, ref.ubo_manager));
		}
		return s_cast<bool>(info);
	}

}