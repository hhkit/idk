#include "pch.h"
#include "MaterialInstanceCache.h"

#include <vkn/ShaderModule.h>

#include <res/ResourceHandle.inl>

#include <vkn/GraphicsState.h>

namespace idk::vkn
{
//#pragma optimize("",off)
	struct MaterialInstanceCache::PImpl
	{
		DescriptorsManager _dm;
		UniformManager ds_builder;

		creation_buffer_t _creation_buffer;
		UboManager _ubo_manager;
		vector <vk::DescriptorImageInfo> scratch;
		vector<vk::DescriptorBufferInfo> buffer_scratch;
		DsUpdater updater;
	};

	struct DescriptorSetDispenser
	{
		CollatedLayouts_t layouts;
	};
	void MaterialInstanceCache::CacheMaterialInstance(const ProcessedMaterial& inst)
	{
		if (inst.shader && inst.shader.as<ShaderModule>().HasCurrent())
		{
			auto& cache = cached_info[inst.inst_guid];
			cache.Update(inst, _pimpl->_creation_buffer, _pimpl->_ubo_manager);
		}
		else
		{
			cached_info.erase(inst.inst_guid);
		}
	}
	span<const MaterialInstanceCache::descriptors_t> MaterialInstanceCache::GetDescriptorSets(RscHandle<MaterialInstance> inst)const
	{
		span<descriptors_t> result{};
		return cached_info.at(inst).descriptors;
	}

	void MaterialInstanceCache::UpdateUniformBuffers()
	{
		_pimpl->_ubo_manager.UpdateAllBuffers();
	}

	struct BindingData
	{
		vk::DescriptorType type;
		uint32_t binding;
		std::optional<vector_span<vk::DescriptorBufferInfo>> data;
		std::optional<vector_span<vk::DescriptorImageInfo>> tex;


		void SetData(vk::Buffer buffer, size_t offset, size_t size, vector_span_builder< vk::DescriptorBufferInfo> builder)
		{
			builder.start();
			builder.emplace_back(vk::DescriptorBufferInfo{
				buffer,offset,size
				});
			data = builder.end();
			tex={};
		}
		void SetTex(span<const RscHandle<Texture>> textures,vector_span_builder< vk::DescriptorImageInfo> builder)
		{
			builder.start();
			builder.grow_reserve(textures.size());
			for (auto& t : textures)
			{
				auto& vtex = t.as<VknTexture>();
				builder.emplace_back(vk::DescriptorImageInfo
					{
						vtex.Sampler(),
						vtex.ImageView(),
						vk::ImageLayout::eGeneral
					}
				);
			}
			tex = builder.end();
			data = {};
		}


		vk::WriteDescriptorSet GetWriteInfo(vk::DescriptorSet ds)const
		{
			vk::WriteDescriptorSet r{
				ds,
				binding,0,
				static_cast<uint32_t>(data ? 1 : tex->size()),
				type,
				(tex) ? tex->to_span().data() : nullptr,
				(data) ? data->to_span().data() : nullptr,
				nullptr
			};
			if (r.descriptorType == vk::DescriptorType::eCombinedImageSampler && r.pImageInfo == nullptr)
				DebugBreak();
			return r;
		}
	};

	struct SetCollation {
		vk::DescriptorSetLayout layout;
		lazy_vector<std::optional<BindingData>> bindings;
	};




	void MaterialInstanceCache::InstCachedInfo::Update(const ProcessedMaterial& mat_inst, creation_buffer_t& creation_buffer,UboManager& ubo_manager)
	{
		auto shader = mat_inst.shader;
		if (!shader || !shader.as<ShaderModule>().HasCurrent())
			return;
		auto& mod = shader.as<ShaderModule>();
		auto compute_pair = [](const ProcessedMaterial& mat_inst)
		{
			size_t data_block_hash = 0;
			hash_combine(data_block_hash, mat_inst.data_block);
			size_t texture_block_hash = 0;
			hash_combine(texture_block_hash, hlp::to_data(mat_inst.texture_block));
			return std::make_pair(data_block_hash, texture_block_hash);
		};
		auto is_exact_same= [this,compute_pair](const ProcessedMaterial& mat_inst)->bool 
		{
			auto [db_hash, tb_hash] = compute_pair(mat_inst);
			return data_hash == db_hash && tb_hash == texture_hash;
		};
		auto shader_is_same = [this](const ProcessedMaterial& mat_inst)->bool
		{
			auto& mod = mat_inst.shader.as<ShaderModule>();
			return mod.HasCurrent() && frag_shader == mod.Module();
		};
		const bool exact_same = is_exact_same(mat_inst);
		const bool same_shader = shader_is_same(mat_inst);
		auto [db_hash, tb_hash] = compute_pair(mat_inst);
		data_hash = db_hash;
		texture_hash = tb_hash;
		frag_shader=mod.Module();
		if (same_shader)
		{
			if (exact_same)
				return;
			auto UpdateDataBuffer = [this](const ProcessedMaterial& mat_inst, UboManager& ubo_manager)->void
			{
				if (buffer_range.size())
					ubo_manager.Update(buffer, buffer_range,mat_inst.data_block);
			};

			return UpdateDataBuffer(mat_inst,ubo_manager);
		}
		//Update the hashes


		creation_buffer.emplace_back(*this, mat_inst);
		
	}

	



	void MaterialInstanceCache::ProcessCreation()
	{
		auto& create_buffer = _pimpl->_creation_buffer;
		UboManager& um= _pimpl->_ubo_manager;
		DescriptorsManager& dm= _pimpl->_dm;
		CollatedLayouts_t clayouts;
		for (auto& [info, mat_inst] : create_buffer)
		{
			ShaderModule& mod = mat_inst.shader.as<ShaderModule>();
			for (auto itr = mod.LayoutsBegin(); itr != mod.LayoutsEnd(); ++itr)
			{
				clayouts[*itr->second.layout].first++;
				clayouts[*itr->second.layout].second = itr->second.entry_counts;
			}
		}
		auto&& allocator = dm.Allocate(clayouts);

		vector_span_builder tbuilder{ _pimpl->scratch };
		vector_span_builder bbuilder{ _pimpl->buffer_scratch };

		vector<std::pair<vk::DescriptorSet, BindingData>> tmp_data;


		for (auto& [info, mat_inst] : create_buffer)
		{
			info.ReleaseDescriptorSets(dm);
			hash_table<set_t, SetCollation> collated_data;
			ShaderModule& mod = mat_inst.shader.as<ShaderModule>();
			auto [buffer, offset] = um.Add(mat_inst.data_block);
			
			for (auto [name, ub] : mat_inst.ubo_table)
			{
				auto& uni_info = mod.GetLayout(name);
				collated_data[uni_info.set].layout = uni_info.layout;
				auto block_offset = ub.data() - mat_inst.data_block.data();
				collated_data[uni_info.set].bindings[uni_info.binding] = BindingData{vk::DescriptorType::eUniformBuffer,uni_info.binding };
				collated_data[uni_info.set].bindings[uni_info.binding]->SetData(buffer,offset+block_offset,ub.size(),bbuilder);
			}
			for (auto [name, textures] : mat_inst.tex_table)
			{
				auto& uni_info = mod.GetLayout(name);
				collated_data[uni_info.set].layout = uni_info.layout;
				collated_data[uni_info.set].bindings[uni_info.binding] = BindingData{ vk::DescriptorType::eCombinedImageSampler,uni_info.binding };
				collated_data[uni_info.set].bindings[uni_info.binding]->SetTex(textures,tbuilder);
			}
			info.sets.clear();
			for (auto& [set, set_data] : collated_data)
			{
				auto ds = allocator.at(set_data.layout).GetNext();
				info.descriptors.emplace_back(set,set_data.layout, ds);
				for (auto binding : set_data.bindings)
				{
					if (binding)
						tmp_data.emplace_back(ds, *binding);
				}
			}
		}
		for (auto& [ds, binding] : tmp_data)
		{
			_pimpl->updater.associate(ds, binding);
		}
		_pimpl->updater.UpdateDescriptorSets();
		_pimpl->_creation_buffer.clear();
	}

	MaterialInstanceCache::MaterialInstanceCache():_pimpl{std::make_unique<PImpl>()}
	{
	}

	MaterialInstanceCache::~MaterialInstanceCache() = default;


	void MaterialInstanceCache::InstCachedInfo::ReleaseDescriptorSets(DescriptorsManager& dm)
	{
		for (auto& [set,layout,ds]: descriptors)
		{
			dm.Free(layout, ds);
		}
		descriptors.clear();
		sets.clear();
	}

	void DsUpdater::associate(vk::DescriptorSet ds, const BindingData& binding)
	{
		write_buffer.emplace_back(binding.GetWriteInfo(ds));
	}

	void DsUpdater::UpdateDescriptorSets()
	{
		View().Device()->updateDescriptorSets(write_buffer, {});
		write_buffer.clear();
	}

}