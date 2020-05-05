#include "pch.h"
#include "UniformManager.h"
#include <vkn/ShaderModule.h>
#include <vkn/Stopwatch.h>
static bool constexpr dbg_uniform = false;
namespace idk::vkn
{
	namespace dbg
	{
		hash_table<string_view, float>& get_rendertask_durations();
		void add_rendertask_durations(string_view name, float duration);

	}
	using dbg::add_rendertask_durations;

	std::optional<UniformUtils::BufferBinding> UniformUtils::BindingInfo::GetBuffer() const
	{
		using T = UniformUtils::BufferBinding;
		std::optional<T> ret;
		if (ubuffer.index() == meta::IndexOf<data_t, T>::value)
			ret = std::get<T>(ubuffer);
		return ret;
	}

	std::optional<UniformUtils::image_t> UniformUtils::BindingInfo::GetImage() const
	{
		using Type = image_t;
		std::optional<Type> ret;
		if (IsImage())
			ret = std::get<Type>(ubuffer);
		return ret;
	}

	std::optional<UniformUtils::AttachmentBinding> UniformUtils::BindingInfo::GetAttachment() const
	{
		using Type = AttachmentBinding;
		std::optional<Type> ret;
		if (IsAttachment())
			ret = std::get<Type>(ubuffer);
		return ret;
	}

	bool UniformUtils::BindingInfo::IsImage() const
	{
		using Type = image_t;
		return ubuffer.index() == meta::IndexOf<data_t, Type>::value;
	}

	bool UniformUtils::BindingInfo::IsAttachment() const
	{
		using Type = AttachmentBinding;
		return ubuffer.index() == meta::IndexOf<data_t, Type>::value;
	}

	vk::DescriptorSetLayout UniformUtils::BindingInfo::GetLayout() const
	{
		return layout;
	}


	struct DSUpdater
	{
		decltype(DescriptorUpdateData::buffer_infos)& buffer_infos;
		DescriptorUpdateData::vector_a<DescriptorUpdateData::vector_a<vk::DescriptorImageInfo>>& image_infos;
		const UniformUtils::BindingInfo& binding;
		const vk::DescriptorSet& dset;
		vk::WriteDescriptorSet& curr;
		void operator()(UniformUtils::BufferBinding& ubuffer)
		{
			//auto& dset = ds2[i++];
			buffer_infos.emplace_front(
				vk::DescriptorBufferInfo{
				  ubuffer.buffer
				, ubuffer.offset
				, binding.size
				}
			);
			;


			curr = vk::WriteDescriptorSet{
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
		void operator()(UniformUtils::image_t ubuffer)
		{
			//auto& dset = ds2[i++];
			auto& bufferInfo = image_infos[binding.binding];
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
		void operator()(UniformUtils::AttachmentBinding ubuffer)
		{
			//auto& dset = ds2[i++];
			auto& bufferInfo = image_infos[binding.binding];
			bufferInfo[binding.arr_index - curr.dstArrayElement] = (
				vk::DescriptorImageInfo{
					{}
				  ,ubuffer.view
				  ,ubuffer.layout
				}
			);
			curr.pImageInfo = std::data(bufferInfo);
			curr.descriptorType = vk::DescriptorType::eInputAttachment;
		}
	};
	void CondenseDSW(DescriptorUpdateData::vector_a<vk::WriteDescriptorSet>& dsw);

	void UpdateUniformDS(
		vk::DescriptorSet& dset,
		span<UniformUtils::BindingInfo> bindings,
		DescriptorUpdateData& out
	)
	{
		auto& buffer_infos = out.scratch_buffer_infos;
		auto& image_infos = out.scratch_image_info;
		auto& descriptorWrite = out.scratch_descriptorWrite;
		using img_vec_t = std::remove_reference_t<decltype(image_infos[0])>;
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
				image_infos.resize(max_binding, img_vec_t(out.alloc));
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
	UniformUtils::BindingInfo::BindingInfo(
		uint32_t binding_,
		vk::Buffer ubuffer_,
		size_t buffer_offset_,
		uint32_t arr_index_,
		size_t size_,
		vk::DescriptorSetLayout layout_
	) :
		binding{ binding_ },
		ubuffer{ BufferBinding{ubuffer_,buffer_offset_ } },
		arr_index{ arr_index_ },
		size{ size_ },
		layout{ layout_ },
		type_index{ desc_type_idx<vk::DescriptorType::eUniformBuffer> }
	{
		if (buffer_offset_ + size_ > 65536 || size_==0)
			throw;
	}
	UniformUtils::BindingInfo::BindingInfo(
		uint32_t binding_,
		image_t ubuffer_,
		uint32_t arr_index_,
		size_t size_,
		vk::DescriptorSetLayout layout_
	) :
		binding{ binding_ },
		ubuffer{ ubuffer_ },
		arr_index{ arr_index_ },
		size{ size_ },
		layout{ layout_ },
		type_index{ desc_type_idx<vk::DescriptorType::eCombinedImageSampler> }
	{
	}
	UniformUtils::BindingInfo::BindingInfo(
		uint32_t binding_,
		AttachmentBinding ubuffer_,
		uint32_t arr_index_,
		size_t size_,
		vk::DescriptorSetLayout layout_
	) :
		binding{ binding_ },
		ubuffer{ ubuffer_ },
		arr_index{ arr_index_ },
		size{ size_ },
		layout{ layout_ },
		type_index{ desc_type_idx<vk::DescriptorType::eInputAttachment> }
	{
	}

	void UniformUtils::binding_manager::set_bindings::SetLayout(vk::DescriptorSetLayout new_layout, const DsCountArray& total_descriptors, bool clear_bindings)
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
	bool UniformUtils::binding_manager::set_bindings::SetOverride(vk::DescriptorSet ds, vk::DescriptorSetLayout layout_)
	{
		const bool valid = layout_ == layout ;
		if (valid)
			ds_override= ds;
		dirty = true;
		return valid;
	}
	
	void UniformUtils::binding_manager::set_bindings::Bind(BindingInfo info)
	{
		//if (bindings.size() <= info.binding)
		//	bindings.resize(static_cast<size_t>(info.binding) + 1);
		dbg::stopwatch timer;
		timer.start();
		auto& vec = bindings[info.binding];
		if (vec.size() <= info.arr_index)
			vec.resize(info.arr_index + 1);
		vec[info.arr_index] = std::move(info);
		dirty = true;
		timer.stop();
		add_rendertask_durations("Bind", timer.time().count());
	}
	void UniformUtils::binding_manager::set_bindings::Unbind(uint32_t binding)
	{
		bindings.erase(binding);
	}
	monadic::result<UniformUtils::binding_manager::binding_span, string> UniformUtils::binding_manager::set_bindings::FinalizeDC(CollatedLayouts_t& collated_layouts, vector_span_builder<BindingInfo>& builder)
	{
		monadic::result< UniformUtils::binding_manager::binding_span, string> result{};
		builder.start();
		auto& set_bindings = builder;
		uint32_t type_count[DescriptorTypeI::size()] = {};
		if (dirty)
		{
			if (ds_override)
			{
				result = *ds_override;
			}
			else
			{

				size_t max_size = 0;
				for (auto& binding : bindings)
				{
					max_size += binding.second.size();
				}
				for (auto& [binding_index, binding] : bindings)
				{
					for (auto& elem : binding)
					{
						if (elem)
						{
							auto& binding_elem = set_bindings.emplace_back(*elem);
							type_count[binding_elem.type_index]++;
						}
					}
				}

				{
					auto& cl = collated_layouts[layout];
					cl.first++;
					for (size_t i = 0; i < std::size(total_desc); ++i)
					{
						cl.second[i] = total_desc[i];
					}
					result = set_bindings.end();
					dirty = false;
				}

			}
		}

		return result;
	}
	void UniformManager::SetUboManager(UboManager& ubo_manager) noexcept
	{
		_ubo_manager = &ubo_manager;
	}
	void UniformManager::AddShader(const ShaderModule& shader)
	{
		for (auto itr = shader.LayoutsBegin(), end = shader.LayoutsEnd(); itr != end; ++itr)
		{
			AddBinding(itr->first, *itr->second.layout, itr->second.entry_counts);
		}
		for (auto itr = shader.InfoBegin(), end = shader.InfoEnd(); itr != end; ++itr)
		{
			auto [name, info] = *itr;
			RegisterUniforms(name, info.set, info.binding, info.size);
		}
	}
	void UniformManager::RemoveShader(const ShaderModule& shader)
	{
		for (auto itr = shader.LayoutsBegin(), end = shader.LayoutsEnd(); itr != end; ++itr)
		{
			RemoveBinding(itr->first);
		}
	}
	void UniformManager::AddBinding(binding_manager::set_t set, vk::DescriptorSetLayout layout, const DsCountArray& counts)
	{
		_bindings.AddBinding(set, layout, counts);
	}

	//Before this

	bool UniformManager::RegisterUniforms(string name, binding_manager::set_t set, uint32_t binding, uint32_t size)
	{
		dbg::stopwatch timer;
		timer.start();
		auto& bindings = _bindings.curr_bindings;
		auto set_info = bindings.find(set);
		bool can_set = set_info != bindings.end();
		if (can_set)
		{
			_uniform_names[std::move(name)] = UniInfo{ set,binding,size,set_info->second.layout };
			_dbg.RegisterRequiredBinding(set, binding);
		}
		timer.stop();
		add_rendertask_durations("Register Uniforms", timer.time().count());
		return can_set;
	}
	void UniformManager::RemoveBinding(binding_manager::set_t set)
	{
		_bindings.RemoveBinding(set);
		for (auto itr = _uniform_names.begin();itr!=_uniform_names.end();)
		{
			auto& [name, info] = *itr;
			if (info.set == set)
				itr = _uniform_names.erase(itr);
			else
				itr++;
		}
		_dbg.RemoveRequiredSet(set);
	}
	namespace detail
	{
		bool is_bound(const UniformManager::set_bindings& existing_bindings, uint32_t binding_index, uint32_t array_index)
		{
			auto& bindings = existing_bindings.bindings;
			auto itr = bindings.find(binding_index);
			return  itr != bindings.end() && itr->second.size() > array_index && itr->second[array_index];
		}
	}
	bool UniformManager::BindDescriptorSet(uint32_t set, vk::DescriptorSet ds, vk::DescriptorSetLayout dsl)
	{
		_dbg.MarkSet(set);
		return _bindings.SetOverride(set, ds,dsl);
	}
	bool UniformManager::BindUniformBuffer(string_view uniform_name, uint32_t array_index, string_view data, bool skip_if_bound)
	{
		auto itr = _uniform_names.find(uniform_name);
		bool bound = false;
		if (itr != _uniform_names.end())
		{
			auto& info = itr->second;
			bound = BindUniformBuffer(info, array_index, data, skip_if_bound);
			if (bound)
				_dbg.MarkBinding(info.set, info.binding);
		}
		return bound;
	}
	bool UniformManager::BindSampler(string_view uniform_name, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		auto itr = _uniform_names.find(uniform_name);
		bool bound = false;
		if (itr != _uniform_names.end())
		{
			auto& info = itr->second;
			bound = BindSampler(info, array_index, texture, skip_if_bound, layout);
			if (bound)
				_dbg.MarkBinding(info.set, info.binding);
		}
		return bound;
	}
	bool UniformManager::BindAttachment(string_view uniform_name, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		auto itr = _uniform_names.find(uniform_name);
		bool bound = false;
		if (itr != _uniform_names.end())
		{
			auto& info = itr->second;
			bound = BindAttachment(info, array_index, texture, skip_if_bound, layout);
			if (bound)
				_dbg.MarkBinding(info.set,info.binding);
		}
		return bound;
	}
	bool UniformManager::BindUniformBuffer(const UniInfo& info, uint32_t array_index, string_view data, bool skip_if_bound)
	{
		dbg::stopwatch timer;
		timer.start();
		auto [buffer, offset] = _ubo_manager->Add(data);
		timer.stop();
		add_rendertask_durations("Ubo Add", timer.time().count());
		return _bindings.BindUniformBuffer(info, array_index, buffer, offset,data.size(), skip_if_bound);
	}
	bool UniformManager::BindSampler(const UniInfo& info, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		return _bindings.BindSampler(info, array_index, texture, skip_if_bound, layout);;
	}
	bool UniformManager::BindAttachment(const UniInfo& info, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		return _bindings.BindAttachment(info, array_index, texture, skip_if_bound, layout);
	}

	namespace visitors
	{
		template<typename T>
		struct FinalizeChecker
		{
			vector_span_builder<T> builder;
			uint32_t index;
			FinalizeChecker(vector_span_builder<T> builder_, uint32_t index_):builder{builder_},index{index_}{}
			void operator()(vk::DescriptorSet ds)
			{
				builder.emplace_back(index, ds);
			}
			template<typename vspan>
			void operator()(vspan bindings)
			{
				if (bindings.size())
					builder.emplace_back(index, bindings);
			}
		};
	}
	std::optional<vector_span<UniformManager::set_binding_t>>  UniformManager::FinalizeCurrent(vector<set_binding_t>& all_sets)
	{
		dbg::stopwatch timer;
		timer.start();
		vector_span_builder builder{ all_sets };
		builder.start();
		
		if (!_dbg.Validate(_bindings))
		{
			LOG_ERROR_TO(LogPool::GFX, "Invalid Bindings");
			_dbg.Validate(_bindings);
			return{};
		}

		for (auto& [index, set] : _bindings.curr_bindings)
		{
			auto bindings = set.FinalizeDC(_collated_layouts,_buffer_builder);
			if (bindings)
			{
				std::visit(visitors::FinalizeChecker{ builder,index }, *bindings);
			}
		}
		timer.stop();
		add_rendertask_durations("finalize current", timer.time().count());
		return builder.end();
	}

	void UniformManager::GenerateDescriptorSets(span<const set_binding_t> bindings, DescriptorUpdateData& dud, DescriptorsManager& dm, vector<vk::DescriptorSet>& descriptor_sets)
	{
		//_dud.Reset();
		auto dsl = dm.Allocate(_collated_layouts);
		size_t i = 0;
		for (auto& [set, bindingv] : bindings)
		{
			vk::DescriptorSet result = {};
			if (bindingv.index() == index_in_variant_v<vk::DescriptorSet, std::decay_t<decltype(bindingv)>>)
			{
				result = std::get<vk::DescriptorSet>(bindingv);
			}
			else
			{
				auto& binding = std::get<vector_span<BindingInfo>>(bindingv);
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
				}(binding);
				auto itr = dsl.find(layout);
				if (itr != dsl.end())
				{
					auto ds = itr->second.GetNext();
					//TODO update ds
					UpdateUniformDS(ds, binding.to_span(), dud);
					result = ds;
				}

			}
			descriptor_sets[i] = result;
			++i;
		}
		//_dud.SendUpdates();
	}
	std::optional<vk::DescriptorSetLayout> UniformUtils::binding_manager::GetLayout(set_t set)
	{
		std::optional<vk::DescriptorSetLayout> result{};
		auto itr = curr_bindings.find(set);
		if (itr != curr_bindings.end())
			result = itr->second.layout;
		return result;
	}
	bool UniformUtils::binding_manager::SetOverride(set_t set, vk::DescriptorSet ds, vk::DescriptorSetLayout dsl)
	{
		bool bound = false;
		auto itr = curr_bindings.find(set);
		if (itr != curr_bindings.end() )
		{
			bound=itr->second.SetOverride(ds,dsl);
		}
		return bound;
	}
	void UniformUtils::binding_manager::AddBinding(set_t set, vk::DescriptorSetLayout layout, const DsCountArray& counts)
	{
		auto b_itr = curr_bindings.find(set);
		if (b_itr == curr_bindings.end())
		{
			curr_bindings[set];
			b_itr = curr_bindings.find(set);
		}
		b_itr->second.SetLayout(layout, counts);
	}
	void UniformUtils::binding_manager::RemoveBinding(set_t set)
	{
		auto b_itr = curr_bindings.find(set);
		if (b_itr != curr_bindings.end())
			curr_bindings.erase(b_itr);
	}
	void UniformUtils::binding_manager::MarkDirty()
	{
		for (auto& [set, bindings] : curr_bindings)
			bindings.dirty = true;
	}
	bool UniformUtils::binding_manager::BindUniformBuffer(UniInfo info, uint32_t array_index, vk::Buffer buffer,size_t offset,size_t size, bool skip_if_bound)
	{
		bool bound = false;

		auto itr2 = curr_bindings.find(info.set);
		if (!skip_if_bound || itr2 == curr_bindings.end() || !detail::is_bound(itr2->second, info.binding, array_index))
		{

			curr_bindings[info.set].Bind(BindingInfo{ info.binding,buffer,offset, array_index, size,info.layout });
			bound = true;
		}
		return bound;
	}
	bool UniformUtils::binding_manager::BindSampler(UniInfo info, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		bool bound = false;
		auto itr2 = curr_bindings.find(info.set);
		if (!skip_if_bound || itr2 == curr_bindings.end() || !detail::is_bound(itr2->second, info.binding, array_index))
		{
			if (texture.ImageView() == vk::ImageView())
				throw;
			curr_bindings[info.set].Bind(BindingInfo{ info.binding,ImageBinding{texture.ImageView(),texture.Sampler(),layout}, array_index, info.size, info.layout });
			bound = true;
		}
		return bound;
	}
	bool UniformUtils::binding_manager::BindAttachment(UniInfo info, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		bool bound = false;
		auto itr2 = curr_bindings.find(info.set);
		if (!skip_if_bound || itr2 == curr_bindings.end() || !detail::is_bound(itr2->second, info.binding, array_index))
		{
			if (texture.ImageView() == vk::ImageView())
				throw;
			curr_bindings[info.set].Bind(BindingInfo{ info.binding,AttachmentBinding{texture.ImageView(),layout}, array_index, info.size, info.layout });
			bound = true;
		}
		return bound;
	}
	bool UniformUtils::binding_manager::is_bound(set_t set, uint32_t binding_index, uint32_t array_index) const noexcept
	{
		const auto s_itr = curr_bindings.find(set);
		if (s_itr == curr_bindings.end())
			return false;

		const set_bindings& existing_bindings = s_itr->second;
		auto& bindings = existing_bindings.bindings;
		auto itr = bindings.find(binding_index);
		return  itr != bindings.end() && itr->second.size() > array_index && itr->second[array_index];
	}
	void UniformManager::DebugInfo::RegisterRequiredBinding(uint32_t set_idx, uint32_t binding_idx)
	{
		if constexpr (!dbg_uniform)
		{
			return;
		}
		auto& set = sets[set_idx];
		auto& binding = set.bindings[binding_idx];
		if(!binding)
			set.bound++;
		binding = true;
	}
	void UniformManager::DebugInfo::RemoveRequiredSet(uint32_t set)
	{
		if constexpr (!dbg_uniform)
		{
			return ;
		}
		sets[set].bindings.clear();
		sets[set].bound =0;
	}
	void UniformManager::DebugInfo::MarkBinding(uint32_t set, uint32_t binding)
	{
		if constexpr (!dbg_uniform)
		{
			return;
		}
		sets[set].bindings[binding] = false;
	}
	//DescriptorUpdateData UniformManager::_dud = {};
	void UniformManager::DebugInfo::MarkSet(uint32_t set)
	{
		if constexpr (!dbg_uniform)
		{
			return;
		}
		for (auto& binding : sets[set].bindings)
		{
			binding = false;
		}
	}
	bool UniformManager::DebugInfo::Validate(const binding_manager& _bindings) const
	{
		if constexpr (!dbg_uniform)
		{
			return true;
		}
		uint32_t set_index = 0;
		for (auto& set : sets)
		{
			if (set.bindings.size())
			{
				auto itr = _bindings.curr_bindings.find(set_index);
				if (itr == _bindings.curr_bindings.end())
				{
					LOG_ERROR_TO(LogPool::GFX, "Required Set #%u not bound", set_index);
					return false;
				}
				auto& curr_set_bindings = itr->second;
				if (set.bound != curr_set_bindings.bindings.size())
					for (uint32_t binding_index = 0; binding_index < set.bindings.size(); ++binding_index)
					{
						auto required = set.bindings[binding_index];
						if (required)
						{
							auto itr2 = curr_set_bindings.bindings.find(binding_index);
							if (itr2 == curr_set_bindings.bindings.end())
							{
								LOG_ERROR_TO(LogPool::GFX, "Required Binding %u in Set #%u not bound", binding_index, set_index);
								return false;
							}
						}
					}
			}
			++set_index;
		}
		return true;
	}
	void UniformManager::DebugInfo::Reset()
	{
		if constexpr (!dbg_uniform)
		{
			return;
		}
		sets.clear();
	}
}