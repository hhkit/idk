#define NOMINMAX
#include "pch.h"
#include "PipelineThingy.h"

#include <vkn/VknCubemap.h>
#include <gfx/RenderObject.h>
#include <vkn/DescriptorsManager.h>
#include <vkn/ShaderModule.h>
#include <forward_list>

#include <vkn/VulkanMesh.h>

#include <vkn/DescriptorUpdateData.h>
#include <ds/result.inl>
#include <res/ResourceHandle.inl>

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
			bufferInfo[binding.arr_index - curr.dstArrayElement] = (
				vk::DescriptorImageInfo{
				  ubuffer.sampler
				  ,ubuffer.view
				  ,ubuffer.layout
				}
			);
			curr.pImageInfo = std::data(bufferInfo);
			curr.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		}
		void operator()(ProcessedRO::AttachmentBinding ubuffer)
		{
			//auto& dset = ds2[i++];
			vector<vk::DescriptorImageInfo>& bufferInfo = image_infos[binding.binding];
			bufferInfo[binding.arr_index - curr.dstArrayElement] = (
				vk::DescriptorImageInfo{
				  ubuffer.sampler
				  ,ubuffer.view
				  ,ubuffer.layout
				}
			);
			curr.pImageInfo = std::data(bufferInfo);
			curr.descriptorType = vk::DescriptorType::eInputAttachment;
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
	/*
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

	*/
	void UpdateUniformDS(
		vk::DescriptorSet& dset,
		vector<ProcessedRO::BindingInfo> bindings,
		DescriptorUpdateData& out
	)
	{
		std::forward_list<vk::DescriptorBufferInfo>& buffer_infos = out.scratch_buffer_infos;
		vector<vector<vk::DescriptorImageInfo>>& image_infos = out.scratch_image_info;
		vector<vk::WriteDescriptorSet> &descriptorWrite = out.scratch_descriptorWrite;
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
			if (binding.IsImage() || binding.IsAttachment())
			{
				image_infos[binding.binding].resize(binding.size, default_img);
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
		out.AbsorbFromScratch();
	}

	ProcessedRO::BindingInfo CreateBindingInfoAtt(const UboInfo& obj_uni, uint32_t arr_index, const VknTexture& val, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		//collated_layouts[obj_uni.layout][desc_type_index<vk::DescriptorType::eCombinedImageSampler>]++;
		//collated_bindings[obj_uni.set].emplace_back(
		return ProcessedRO::BindingInfo
		{
			obj_uni.binding,
			ProcessedRO::AttachmentBinding{ val.ImageView(),val.Sampler(),layout},
			0,
			arr_index,
			obj_uni.size,
			obj_uni.layout
		};
		//);
	}


	ProcessedRO::BindingInfo CreateBindingInfo(const UboInfo& obj_uni, uint32_t arr_index, const VknTexture& val, vk::ImageLayout layout = vk::ImageLayout::eGeneral)
	{
		//collated_layouts[obj_uni.layout][desc_type_index<vk::DescriptorType::eCombinedImageSampler>]++;
		//collated_bindings[obj_uni.set].emplace_back(
		return ProcessedRO::BindingInfo
		{
			obj_uni.binding,
			ProcessedRO::ImageBinding{ val.ImageView(),*val.sampler,layout },
			0,
			arr_index,
			obj_uni.size,
			obj_uni.layout
		};
		//);
	}

	ProcessedRO::BindingInfo CreateBindingInfo(const UboInfo& obj_uni, uint32_t arr_index, const VknCubemap& val, vk::ImageLayout layout = vk::ImageLayout::eGeneral)
	{
		//collated_layouts[obj_uni.layout][desc_type_index<vk::DescriptorType::eCombinedImageSampler>]++;
		//collated_bindings[obj_uni.set].emplace_back(
		return ProcessedRO::BindingInfo
		{
			obj_uni.binding,
			ProcessedRO::ImageBinding{ val.ImageView(),val.Sampler(),layout },
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
			if(mod)
			{ 
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
					b_itr->second.SetLayout(*layout,layout.entry_counts);
				}
				for (auto& set : curr_bindings)
				{
					set.second.dirty = true;
				}
				shaders[static_cast<size_t>(stage)] = shader;
				shader_changed = true;

			}
		}
	}
	void PipelineThingy::BindAttrib(uint32_t location, vk::Buffer buffer, size_t offset)
	{
		attrib_buffers[location] = { buffer,offset };
	}
	bool PipelineThingy::BindMeshBuffers(const RenderObject& ro)
	{
		return BindMeshBuffers(ro.mesh, *ro.renderer_req);
	}
	bool PipelineThingy::BindMeshBuffers(RscHandle<Mesh> mesh, const renderer_attributes& attribs)
	{
		auto& vmesh = mesh.as<VulkanMesh>();
		return BindMeshBuffers(vmesh, attribs);
	}
	bool PipelineThingy::BindMeshBuffers(const VulkanMesh& mesh, const renderer_attributes& attribs)
	{
		for (auto&& [attrib, location] : attribs.mesh_requirements)
		{
			if (!mesh.Has(attrib))
				return false;
			auto& attrib_buffer = mesh.Get(attrib);
			BindAttrib(location, *attrib_buffer.buffer(), attrib_buffer.offset);
		}
		auto& vmesh = mesh;
		auto& idx_buffer = vmesh.GetIndexBuffer();
		if (idx_buffer && idx_buffer->buffer())
		{
			index_buffer = BoundIndexBuffer{ *idx_buffer->buffer(),idx_buffer->offset,vmesh.IndexType() };
			num_vertices = vmesh.IndexCount();
		};
		return true;
	}
	void PipelineThingy::SetVertexCount(uint32_t vertex_count)
	{
		num_vertices = vertex_count;
	}
	std::optional<UboInfo> PipelineThingy::GetUniform(const string& uniform_name) const
	{
		std::optional<UboInfo> result{};
		for (auto& ohshader : shaders)
		{
			if (ohshader)
			{
				auto& shader = ohshader->as<ShaderModule>();
				if (shader && shader.HasLayout(uniform_name))
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
	bool PipelineThingy::BindSampler(const string& uniform_name, uint32_t array_index, const VknTexture& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		auto info = GetUniform(uniform_name);
		if (info)
		{
			auto itr = curr_bindings.find(info->set);
			if (!skip_if_bound || itr == curr_bindings.end() || !detail::is_bound(itr->second,info->binding,array_index))
				curr_bindings[info->set].Bind(CreateBindingInfo(*info, array_index, texture,layout));
		}
		return s_cast<bool>(info);
	}
	bool PipelineThingy::BindAttachment(const string& uniform_name, uint32_t array_index, const VknTexture& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		auto info = GetUniform(uniform_name);
		if (info)
		{
			auto itr = curr_bindings.find(info->set);
			if (!skip_if_bound || itr == curr_bindings.end() || !detail::is_bound(itr->second, info->binding, array_index))
				curr_bindings[info->set].Bind(CreateBindingInfoAtt(*info, array_index, texture,layout));
		}
		return s_cast<bool>(info);
	}
	bool PipelineThingy::BindSampler(const string& uniform_name, uint32_t array_index, const VknCubemap& texture, bool skip_if_bound, vk::ImageLayout layout )
	{
		auto info = GetUniform(uniform_name);
		if (info)
		{
			auto itr = curr_bindings.find(info->set);
			if (!skip_if_bound || itr == curr_bindings.end() || !detail::is_bound(itr->second, info->binding, array_index))
				curr_bindings[info->set].Bind(CreateBindingInfo(*info, array_index, texture,layout));
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
		shared_ptr<const pipeline_config> next_config{};
		if (ro.config != prev_config)
		{
			prev_config = next_config = ro.config;
		}
		auto& p_ro = draw_calls.emplace_back(ProcessedRO{ &ro,std::move(attrib_buffers), std::move(index_buffer),num_vertices,std::move(sets),next_config,shaders[static_cast<size_t>(ShaderStage::Vertex)],shaders[static_cast<size_t>(ShaderStage::Geometry)],shaders[static_cast<size_t>(ShaderStage::Fragment)] });
		p_ro.rebind_shaders = shader_changed;
		p_ro.config = ro.config;
		attrib_buffers = {};
		index_buffer = {};
		num_vertices = 0;
		shader_changed = false;
	}
	void PipelineThingy::FinalizeDrawCall(const RenderObject& ro, size_t num_inst, size_t inst_offset)
	{
		FinalizeDrawCall(ro);
		draw_calls.back().num_instances = num_inst;
		draw_calls.back().inst_offset = inst_offset;
	}
	void PipelineThingy::GenerateDS(DescriptorsManager& d_manager,bool update_ubo_buffers)
	{
		auto dsl = d_manager.Allocate(ref.collated_layouts);
		if (update_ubo_buffers)
			UpdateUboBuffers();
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
				UpdateUniformDS( ds, bindings,dud);
				p_ro.SetDescriptorSet(set,ds);
			}
		}
		dud.SendUpdates();
	}
	void PipelineThingy::UpdateUboBuffers()
	{
		ref.ubo_manager.UpdateAllBuffers();
	}
	//reserves an extra size chunk
	void PipelineThingy::reserve(size_t size)
	{
		draw_calls.reserve(draw_calls.size()+size);
	}
	void PipelineThingy::set_bindings::SetLayout(vk::DescriptorSetLayout new_layout, const DsCountArray& total_descriptors, bool clear_bindings)
	{
		//If the layouts are different, the bindings are not compatible, clear it.
		//or we just want the bindings to be cleared, so clear it.
		if (new_layout != layout || clear_bindings)
		{
			total_desc = total_descriptors;
			bindings.clear();
		}
		layout = new_layout;
	}
	void PipelineThingy::set_bindings::Bind(ProcessedRO::BindingInfo info)
	{
		//if (bindings.size() <= info.binding)
		//	bindings.resize(static_cast<size_t>(info.binding) + 1);
		auto& vec = bindings[info.binding];
		if (vec.size() <= info.arr_index)
			vec.resize(info.arr_index + 1);
		vec[info.arr_index] = std::move(info);
		dirty = true;
	}
	void PipelineThingy::set_bindings::Unbind(uint32_t binding)
	{
		bindings.erase(binding);
	}
	monadic::result<vector<ProcessedRO::BindingInfo>, string> PipelineThingy::set_bindings::FinalizeDC(CollatedLayouts_t& collated_layouts)
	{
		monadic::result< vector<ProcessedRO::BindingInfo>, string> result{};

		string err_msg;
		vector<ProcessedRO::BindingInfo>& set_bindings = scratch_out;
		set_bindings.clear();
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
			for (auto& [binding_index, binding] : bindings)
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
				for (size_t i = 0; i < std::size(total_desc); ++i)
				{
					cl.second[i] = total_desc[i];
				}
				result = std::move(set_bindings);
				dirty = false;
			}
		}

		return std::move(result);
	}
}