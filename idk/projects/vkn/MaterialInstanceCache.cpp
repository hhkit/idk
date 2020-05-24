#include "pch.h"
#include "MaterialInstanceCache.h"

#include <vkn/ShaderModule.h>

#include <res/ResourceHandle.inl>

#include <vkn/GraphicsState.h>

namespace idk::vkn
{

	struct MaterialInstanceCache::PImpl
	{
		DescriptorsManager _dm;

		creation_buffer_t _creation_buffer;
		UboManager _ubo_manager;
		vector <vk::DescriptorImageInfo> scratch;
		vector<vk::DescriptorBufferInfo> buffer_scratch;
		DsUpdater updater;

		hash_set<Guid> processed;

		hash_set<RscHandle<MaterialInstance>> actives;

	};

	struct SetCollation {
		vk::DescriptorSetLayout layout;
		lazy_vector<std::optional<BindingData>> bindings;
	};


	struct MaterialInstanceCache::UpdateInfo
	{
		creation_buffer_t& create_buffer;
		UboManager& ubo_manager;
		DsUpdater& ds_updater;
		vector <vk::DescriptorImageInfo>& scratch;
	};
	struct DescriptorSetDispenser
	{
		CollatedLayouts_t layouts;
	};
	void MaterialInstanceCache::Start()
	{
		//_pimpl->processed.clear();
	}
	void MaterialInstanceCache::SetTotal(hash_set<RscHandle<MaterialInstance>> materials)
	{
		//_pimpl->actives = std::move(materials);
	}
	void MaterialInstanceCache::CacheMaterialInstance(const ProcessedMaterial& inst)
	{
		if (inst.shader)
		{
			auto& mod = inst.shader.as<ShaderModule>();
			if (mod.HasCurrent())
			{
				try {

				auto& cache = cached_info[inst.inst_guid];
				UpdateInfo ui{
					_pimpl->_creation_buffer,
					_pimpl->_ubo_manager,
					_pimpl->updater,
					_pimpl->scratch};
				//_pimpl->processed.emplace(inst.inst_guid);
				cache.Update(inst,ui );
				return;

				}
				catch (...)
				{
					auto guid = inst.inst_guid.operator idk::string();
					LOG_ERROR_TO(LogPool::GFX, "Material %s and shader mismatch!",guid.c_str());
				}
			}
		}
		if(cached_info.find(inst.inst_guid)!= cached_info.end())
			cached_info.erase(inst.inst_guid);
	}
	bool MaterialInstanceCache::IsCached(RscHandle<MaterialInstance> mat_inst) const
	{
		//bool result = _pimpl->processed.find(mat_inst.guid) != _pimpl->processed.end();
		//if (!result)
		//	DebugBreak();
		return cached_info.find(mat_inst) != cached_info.end();//&& result;
	}
	namespace dbg
	{
		void UpdateUsage(vk::DescriptorSet ds);
		void UpdateUsage(vk::Sampler sampler, vk::DescriptorSet ds);
		hash_table<void*, size_t>& get_tracker();
		size_t get_counter();
		bool GetValid(vk::DescriptorSet ds);
	}
	span<const MaterialInstanceCache::descriptors_t> MaterialInstanceCache::GetDescriptorSets(RscHandle<MaterialInstance> inst)const
	{
		//if (_pimpl->processed.find(inst.guid) == _pimpl->processed.end())
		//	DebugBreak();
		span<const descriptors_t> result = cached_info.at(inst).descriptors;
		//for (auto& [i,dsl,ds] : result)
		//{
		//	if (!dbg::GetValid(ds))
		//		DebugBreak();
		//}
		return result;
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
		std::optional<vector_span<vk::DescriptorImageInfo> > tex;


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
			//if (tex)
			//{
			//	for (auto& t : tex->to_span())
			//	{
			//		dbg::UpdateUsage(t.sampler,ds);
			//	}
			//}
			vk::WriteDescriptorSet r{
				ds,
				binding,0,
				static_cast<uint32_t>(data ? 1 : tex->size()),
				type,
				(tex) ? tex->to_span().data() : nullptr,
				(data) ? data->to_span().data() : nullptr,
				nullptr
			};
			return r;
		}
	};
	//namespace dbg
	//{
	//	size_t get_counter();
	//}
#pragma optimize("",off)
	void MaterialInstanceCache::InstCachedInfo::Update(const ProcessedMaterial& mat_inst, UpdateInfo& update_info)
	{
		auto& creation_buffer = update_info.create_buffer;
		auto& ubo_manager = update_info.ubo_manager;
		auto& ds_updater = update_info.ds_updater;
		vector_span_builder tbuilder{ update_info.scratch };
		auto shader = mat_inst.shader;
		if (!shader || !shader.as<ShaderModule>().HasCurrent())
			return;
		auto& mod = shader.as<ShaderModule>();
		auto compute_pair = [](const InstCachedInfo& cached)
		{
			
			std::hash<string> hasher;
			size_t data_block_hash = data_block_hash = hasher(cached.scratch_data_cache);
			size_t texture_block_hash = hasher(hlp::to_data(cached.scratch_texture_cache));
			return std::make_pair(data_block_hash, texture_block_hash);
		};
		auto convert_to_images = [](vector<vk::Image>& textures, const ProcessedMaterial& mat_inst)
		{
			textures.resize(mat_inst.texture_block.size());
			std::transform(mat_inst.texture_block.begin(), mat_inst.texture_block.end(), textures.begin(), [](RscHandle<Texture> tex)
				{
					return tex.as<VknTexture>().Image();
				});
		};
		auto update_texture_scratch = [convert_to_images](InstCachedInfo& cached, const ProcessedMaterial& mat_inst)->void
		{
			thread_local static vector<vk::Image> textures;
			convert_to_images(textures, mat_inst);
			auto sz = hlp::buffer_size(textures);
			auto& buf = cached.scratch_texture_cache;
			buf.resize(sz);
			std::memcpy(hlp::buffer_data(buf), hlp::buffer_data(textures), sz);
		};
		auto update_data_scratch = [](InstCachedInfo& cached, const ProcessedMaterial& mat_inst)->void
		{
			cached.scratch_data_cache = mat_inst.data_block;
		};

		update_data_scratch(*this, mat_inst);
		update_texture_scratch(*this, mat_inst);
		auto is_exact_same= [this,compute_pair](const ProcessedMaterial& mat_inst)->bool 
		{
			auto [db_hash, tb_hash] = compute_pair(*this);

			bool is_hash_same = data_hash == db_hash && tb_hash == texture_hash;

			return is_hash_same && scratch_data_cache == data_cache && scratch_texture_cache == texture_cache;
		};
		auto shader_is_same = [this](const ProcessedMaterial& mat_inst)->bool
		{
			auto& mod = mat_inst.shader.as<ShaderModule>();
			return mod.HasCurrent() && frag_shader == mod.Module();
		};
		const bool exact_same = is_exact_same(mat_inst);
		const bool same_shader = shader_is_same(mat_inst);
		//Update the hashes
		auto [db_hash, tb_hash] = compute_pair(*this);
		data_hash = db_hash;
		texture_hash = tb_hash;
		std::swap(data_cache, scratch_data_cache);
		std::swap(texture_cache, scratch_texture_cache);
		frag_shader=mod.Module();
		if (same_shader)
		{
			if (exact_same)
			{
				size_t i = 0;
				for (auto& tex : mat_inst.texture_block)
				{
					auto& t = tex.as<VknTexture>();
					if (t.Image() != image_cache.at(i++))
						DebugBreak();
				}
				return;
			}
			auto UpdateDataBuffer = [this](const ProcessedMaterial& mat_inst, UboManager& ubo_manager)->void
			{
				for(auto& binfo : buffers)
				{
					if (binfo.buffer_range.size())
					{
						ubo_manager.Update(binfo.buffer, binfo.buffer_range, mat_inst.data_block.substr(binfo.data_range._begin,binfo.data_range._end));;
					}
				}
			};
			for (auto& [name,textures] : mat_inst.tex_table)
			{
				auto& info =mod.GetLayout(name);
				if (!&info)
					throw std::runtime_error("Invalid material");
				BindingData bd{vk::DescriptorType::eCombinedImageSampler,info.binding};
				bd.SetTex(textures,tbuilder);
				auto& [set,dsl,ds] = descriptors.at(descriptor_indices.at(info.set));
				ds_updater.associate(ds,bd);
			}
			convert_to_images(image_cache,mat_inst);
			return UpdateDataBuffer(mat_inst,ubo_manager);
		}


		convert_to_images(image_cache, mat_inst);
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
			for (auto [name, ub] : mat_inst.ubo_table)
			{
				auto& uni_info = mod.GetLayout(name);
				collated_data[uni_info.set].layout = uni_info.layout;
				
				size_t block_offset = ub.data() - mat_inst.data_block.data();

				auto [buffer, offset] = um.Add(mat_inst.data_block.substr(block_offset,ub.size()));
				;
				info.buffers.emplace_back(InstCachedInfo::BufferInfo{ buffer,index_span{offset,offset + ub.size()},index_span{block_offset,block_offset + ub.size()} });
				//info.buffer_range = { offset,offset + mat_inst.data_block.size() };

				collated_data[uni_info.set].bindings[uni_info.binding] = BindingData{vk::DescriptorType::eUniformBuffer,uni_info.binding };
				collated_data[uni_info.set].bindings[uni_info.binding]->SetData(buffer,offset,ub.size(),bbuilder);
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
				auto index = info.descriptors.size();
				info.descriptors.emplace_back(set,set_data.layout, ds);
				info.descriptor_indices.emplace(set, index);
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
		_pimpl->scratch.clear();
		_pimpl->buffer_scratch.clear();
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
		descriptor_indices.clear();
		sets.clear();
	}

	void DsUpdater::associate(vk::DescriptorSet ds, const BindingData& binding)
	{
		write_buffer.emplace_back(binding.GetWriteInfo(ds));

		auto& write_info = write_buffer.back();
		if (write_info.pBufferInfo)
		{
			auto offset = b_scratch.size();
			std::copy(write_info.pBufferInfo, write_info.pBufferInfo + write_info.descriptorCount, std::back_inserter(b_scratch));
			write_info.pBufferInfo = (vk::DescriptorBufferInfo*) offset;
		}

		if (write_info.pImageInfo)
		{
			auto offset = i_scratch.size();
			std::copy(write_info.pImageInfo, write_info.pImageInfo + write_info.descriptorCount, std::back_inserter(i_scratch));
			write_info.pImageInfo = (vk::DescriptorImageInfo*) offset;
		}

	}
	void DsUpdater::UpdateDescriptorSets()
	{
		for (auto& write_info : write_buffer)
		{
			if (write_info.descriptorType == vk::DescriptorType::eUniformBuffer
				||
				write_info.descriptorType == vk::DescriptorType::eStorageBuffer
				||
				write_info.descriptorType == vk::DescriptorType::eUniformBufferDynamic
				|| 
				write_info.descriptorType == vk::DescriptorType::eStorageBufferDynamic)
			{
				write_info.pBufferInfo = b_scratch.data() +  (size_t)write_info.pBufferInfo;
			}

			if (write_info.descriptorType == vk::DescriptorType::eCombinedImageSampler)
			{
				write_info.pImageInfo = i_scratch.data() + (size_t)write_info.pImageInfo;
			}
			
		}
		View().Device()->updateDescriptorSets(write_buffer, {});
		write_buffer.clear();
		b_scratch.clear();
		i_scratch.clear();
	}

}