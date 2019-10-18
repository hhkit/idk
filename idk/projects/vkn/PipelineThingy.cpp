#define NOMINMAX
#include "pch.h"
#include "PipelineThingy.h"

#include <gfx/RenderObject.h>
#include <vkn/DescriptorsManager.h>
#include <vkn/ShaderModule.h>
namespace idk::vkn
{
	VulkanView& View();
	struct DSUpdater
	{
		std::forward_list<vk::DescriptorBufferInfo>& buffer_infos;
		vector<vector<vk::DescriptorImageInfo>>& image_infos;
		const ProcessedRO::BindingInfo& binding;
		const vk::DescriptorSet& dset;
		vk::WriteDescriptorSet& curr;
		void operator()(vk::Buffer ubuffer)
		{
			//auto& dset = ds2[i++];
			buffer_infos.emplace_front(
				vk::DescriptorBufferInfo{
				  ubuffer
				, binding.buffer_offset
				, binding.size
				}
			);
			;

			
			curr=vk::WriteDescriptorSet{
					dset
					,binding.binding
					,binding.arr_index
					,1
					,vk::DescriptorType::eUniformBuffer
					,nullptr
					,&buffer_infos.front()
					,nullptr
			}
			;
		}
		void operator()(ProcessedRO::image_t ubuffer)
		{
			//auto& dset = ds2[i++];
			vector<vk::DescriptorImageInfo>& bufferInfo = image_infos[binding.binding];
			bufferInfo[binding.arr_index-curr.dstArrayElement]=(
				vk::DescriptorImageInfo{
				  ubuffer.sampler
				  ,ubuffer.view
				  ,ubuffer.layout
				}
			);
			curr.pImageInfo = std::data(bufferInfo);
			curr.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		}
	};
	void CondenseDSW(vector<vk::WriteDescriptorSet>& dsw)
	{
		auto insert_itr = dsw.begin();
		for (auto itr = dsw.begin(); itr != dsw.end(); ++itr)
		{
			if (itr->descriptorCount != 0)
			{
				if (itr != insert_itr)
					std::swap((*(insert_itr)), (*itr));
				++insert_itr;
			}
		}
		dsw.resize(insert_itr - dsw.begin());
	}
	void UpdateUniformDS(
		vk::Device& device,
		vk::DescriptorSet& dset,
		vector<ProcessedRO::BindingInfo> bindings
	)
	{
		std::forward_list<vk::DescriptorBufferInfo> buffer_infos;
		vector<vector<vk::DescriptorImageInfo>> image_infos;
		vector<vk::WriteDescriptorSet> descriptorWrite;
		uint32_t max_binding = 0;
		VknTexture& def = RscHandle<VknTexture>{}.as<VknTexture>();
		vk::DescriptorImageInfo default_img
		{
			def.Sampler(),def.ImageView(),vk::ImageLayout::eGeneral,
		};

		for (auto& binding : bindings)
		{
			max_binding = std::max(binding.binding + 1, max_binding);
			if (max_binding > descriptorWrite.size())
			{
				descriptorWrite.resize(max_binding, vk::WriteDescriptorSet{});
				image_infos.resize(max_binding);
			}
			auto& curr = descriptorWrite[binding.binding];
			if (binding.IsImage())
			{
				image_infos[binding.binding].resize(binding.size,default_img);
				curr.descriptorCount = static_cast<uint32_t>(binding.size);
			}
			curr.dstSet = dset;
			curr.dstBinding = binding.binding;
			curr.dstArrayElement = 0;
		}
		//TODO: Handle Other DSes as well
		for (auto& binding : bindings)
		{
			DSUpdater updater{ buffer_infos,image_infos,binding,dset,descriptorWrite[binding.binding] };
			std::visit(updater, binding.ubuffer);
		}
		CondenseDSW(descriptorWrite);
		device.updateDescriptorSets(descriptorWrite, nullptr, vk::DispatchLoaderDefault{});
	}

	ProcessedRO::BindingInfo CreateBindingInfo(const UboInfo& obj_uni, uint32_t arr_index, const VknTexture& val)
	{
		//collated_layouts[obj_uni.layout][desc_type_index<vk::DescriptorType::eCombinedImageSampler>]++;
		//collated_bindings[obj_uni.set].emplace_back(
		return ProcessedRO::BindingInfo
		{
			obj_uni.binding,
			ProcessedRO::ImageBinding{ val.ImageView(),*val.sampler,vk::ImageLayout::eGeneral },
			0,
			arr_index,
			obj_uni.size,
			obj_uni.layout
		};
		//);
	}
	void PipelineThingy::SetRef(UboManager& ubo_manager)
	{
		ref.~Ref();
		new (&ref) Ref{ &ubo_manager };
	}
	void PipelineThingy::UnbindShader(ShaderStage stage)
	{
		auto& omod = shaders[static_cast<size_t>(stage)];
		if (omod)
		{
			auto& mod = omod->as<ShaderModule>();
			//Update existing bindings
			for (auto l_itr = mod.LayoutsBegin(); l_itr != mod.LayoutsEnd(); ++l_itr)
			{
				auto& [set, layout] = *l_itr;
				auto b_itr = curr_bindings.find(set);
				if (b_itr != curr_bindings.end())
					curr_bindings.erase(b_itr);

			}
			shader_changed = true;
		}
		shaders[static_cast<size_t>(stage)] = std::nullopt;

	}
	void PipelineThingy::BindShader(ShaderStage stage, RscHandle<ShaderProgram> shader)
	{
		auto& out_shader = shaders[static_cast<size_t>(stage)];
		if (!out_shader || shader != out_shader)
		{
			UnbindShader(stage);
			auto& mod = shader.as<ShaderModule>();
			//Update existing bindings
			for (auto l_itr = mod.LayoutsBegin(); l_itr != mod.LayoutsEnd(); ++l_itr)
			{
				auto& [set, layout] = *l_itr;
				auto b_itr = curr_bindings.find(set);
				if (b_itr == curr_bindings.end())
				{
					curr_bindings[set];
					b_itr = curr_bindings.find(set);
				}
				b_itr->second.SetLayout(*layout);
			}
			for (auto& set : curr_bindings)
			{
				set.second.dirty = true;
			}
			shaders[static_cast<size_t>(stage)] = shader;
			shader_changed = true;
		}
	}
	std::optional<UboInfo> PipelineThingy::GetUniform(const string& uniform_name) const
	{
		std::optional<UboInfo> result{};
		for (auto& ohshader : shaders)
		{
			if (ohshader)
			{
				auto& shader = ohshader->as<ShaderModule>();
				if (shader.HasLayout(uniform_name))
				{
					result = shader.GetLayout(uniform_name);
					break;
				}
			}
		}
		return result;
	}
	namespace detail
	{
	bool is_bound(PipelineThingy::set_bindings & existing_bindings, uint32_t binding_index, uint32_t array_index)
	{
		auto& bindings = existing_bindings.bindings;
		auto itr = bindings.find(binding_index);
		return  itr != bindings.end() && itr->second.size() > array_index && itr->second[array_index];
	}
	}
	bool PipelineThingy::BindSampler(const string& uniform_name, uint32_t array_index, const VknTexture& texture, bool skip_if_bound)
	{
		auto info = GetUniform(uniform_name);
		if (info)
		{
			auto itr = curr_bindings.find(info->set);
			if (!skip_if_bound || itr == curr_bindings.end() || !detail::is_bound(itr->second,info->binding,array_index))
				curr_bindings[info->set].Bind(CreateBindingInfo(*info, array_index, texture));
		}
		return s_cast<bool>(info);
	}
	void PipelineThingy::FinalizeDrawCall(const RenderObject& ro)
	{
		hash_table<uint32_t, vector<ProcessedRO::BindingInfo>> sets;
		for (auto& [set, bindings] : curr_bindings)
		{
			auto result = bindings.FinalizeDC(ref.collated_layouts);
			if (result) //If binding was valid
			{
				if (result.value().size())
					sets[set] = std::move(result.value());
			}
			//else
			//{
			//	hlp::cerr() << "Failed to bind set[" << set << "]: {" << result.error() << "}\n";
			//}
		}
		shared_ptr<pipeline_config> next_config{};
		if (ro.config != prev_config)
		{
			prev_config = next_config = ro.config;
		}
		auto& p_ro = draw_calls.emplace_back(ProcessedRO{ &ro,std::move(sets),next_config,shaders[static_cast<size_t>(ShaderStage::Vertex)],shaders[static_cast<size_t>(ShaderStage::Geometry)],shaders[static_cast<size_t>(ShaderStage::Fragment)] });
		p_ro.rebind_shaders = shader_changed;
		shader_changed = false;
	}
	void PipelineThingy::GenerateDS(DescriptorsManager& d_manager)
	{
		auto dsl = d_manager.Allocate(ref.collated_layouts);
		ref.ubo_manager.UpdateAllBuffers();
		for (auto& p_ro : draw_calls)
		{
			for (auto& [set, bindings] : p_ro.bindings)
			{
				auto layout = [](auto& bindings)
				{
					vk::DescriptorSetLayout layout{};
					for (auto& binding : bindings)
					{
						if (binding.layout)
						{
							layout = binding.layout;
							break;
						}
					}
					return layout;
					//bindings.front().layout;
				}(bindings);
				auto& ds = dsl.find(layout)->second.GetNext();
				vk::Device device = *View().Device();
				UpdateUniformDS(device, ds, bindings);
				p_ro.descriptor_sets[set]=ds;
			}
		}
	}
}