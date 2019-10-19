#pragma once
#include <idk.h>
#include <vulkan/vulkan.hpp>
#include <vkn/ProcessedRO.h>
#include <gfx/pipeline_config.h>

#include <vkn/vulkan_enum_info.h>
#include <vkn/UboManager.h>
#include <vkn/ShaderModule.h> //UboInfo
#include <vkn/VulkanHashes.h>
namespace idk::vkn
{
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
			void SetLayout(vk::DescriptorSetLayout new_layout, bool clear_bindings = false)
			{
				//If the layouts are different, the bindings are not compatible, clear it.
				//or we just want the bindings to be cleared, so clear it.
				if (new_layout != layout || clear_bindings)
				{
					bindings.clear();
				}
				layout = new_layout;
			}
			void Bind(ProcessedRO::BindingInfo info)
			{
				//if (bindings.size() <= info.binding)
				//	bindings.resize(static_cast<size_t>(info.binding) + 1);
				auto& vec = bindings[info.binding];
				if (vec.size() <= info.arr_index)
					vec.resize(info.arr_index + 1);
				vec[info.arr_index] = std::move(info);
				dirty = true;
			}
			void Unbind(uint32_t binding)
			{
				bindings.erase(binding);
			}
			monadic::result< vector<ProcessedRO::BindingInfo>, string> FinalizeDC(CollatedLayouts_t& collated_layouts)
			{
				monadic::result< vector<ProcessedRO::BindingInfo>, string> result{};

				string err_msg;
				vector<ProcessedRO::BindingInfo> set_bindings;
				uint32_t type_count[DescriptorTypeI::size()] = {};
				bool failed = false;
				if (dirty)
				{
					size_t max_size = 0;
					for (auto& binding : bindings)
					{
						max_size += binding.second.size();
					}
					set_bindings.reserve(max_size);
					for (auto& [binding_index,binding] : bindings)
					{
						//if (!bindings[i])
						//{
						//	failed = true;
						//	err_msg += "Binding [" + std::to_string(i) + "] is missing\n";
						//}
						for (auto& elem : binding)
						{
							if (elem)
							{
								auto& binding_elem = set_bindings.emplace_back(*elem);
								type_count[binding_elem.IsImage() ?
									desc_type_index<vk::DescriptorType::eCombinedImageSampler>
									:
									desc_type_index<vk::DescriptorType::eUniformBuffer>]++;
							}
						}
					}
					if (failed)
						result = std::move(err_msg);
					else
					{
						auto& cl = collated_layouts[layout];
						cl.first++;
						for (size_t i = 0; i < std::size(type_count); ++i)
						{
							cl.second[i] = type_count[i];
						}
						result = std::move(set_bindings);
						dirty = false;
					}
				}
				return std::move(result);
			}
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

		void FinalizeDrawCall(const RenderObject& ro);

		void GenerateDS(DescriptorsManager& d_manager);

		const vector<ProcessedRO>& DrawCalls()const
		{
			return draw_calls;
		}

	private:
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
		vector<ProcessedRO::BindingInfo> bindings
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