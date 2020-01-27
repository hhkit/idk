#include "pch.h"
#include "UniformManager.h"

namespace idk::vkn
{

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
		type_index{ desc_type_index<vk::DescriptorType::eUniformBuffer> }
	{
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
		type_index{ desc_type_index<vk::DescriptorType::eCombinedImageSampler> }
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
		type_index{ desc_type_index<vk::DescriptorType::eInputAttachment> }
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
	void UniformUtils::binding_manager::set_bindings::Bind(BindingInfo info)
	{
		//if (bindings.size() <= info.binding)
		//	bindings.resize(static_cast<size_t>(info.binding) + 1);
		auto& vec = bindings[info.binding];
		if (vec.size() <= info.arr_index)
			vec.resize(info.arr_index + 1);
		vec[info.arr_index] = std::move(info);
		dirty = true;
	}
	void UniformUtils::binding_manager::set_bindings::Unbind(uint32_t binding)
	{
		bindings.erase(binding);
	}
	monadic::result<vector_span<UniformUtils::BindingInfo>, string> UniformUtils::binding_manager::set_bindings::FinalizeDC(CollatedLayouts_t& collated_layouts, vector_span_builder<BindingInfo>& builder)
	{
		monadic::result< vector_span<BindingInfo>, string> result{};
		builder.start();
		string err_msg;
		auto& set_bindings = builder;
		uint32_t type_count[DescriptorTypeI::size()] = {};
		bool failed = false;
		if (dirty)
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
				result = set_bindings.end();
				dirty = false;
			}
		}

		return result;
	}
	void UniformManager::AddBinding(binding_manager::set_t set, vk::DescriptorSetLayout layout, const DsCountArray& counts)
	{
		_bindings.AddBinding(set, layout, counts);
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
	bool UniformManager::BindUniformBuffer(string_view uniform_name, uint32_t array_index, string_view data, bool skip_if_bound)
	{
		auto itr = _uniform_names.find(uniform_name);
		bool bound = false;
		if (itr != _uniform_names.end())
		{
			auto& info = itr->second;
			auto [buffer, offset] = _ubo_manager->Add(data);
			_bindings.BindUniformBuffer(info, array_index, buffer, offset, skip_if_bound);
		}
		return bound;
	}
	bool UniformManager::BindSampler(const string& uniform_name, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		auto itr = _uniform_names.find(uniform_name);
		bool bound = false;
		if (itr != _uniform_names.end())
		{
			auto& info = itr->second;
			bound = _bindings.BindSampler(info, array_index, texture, skip_if_bound, layout);
		}
		return bound;
	}
	bool UniformManager::BindAttachment(const string& uniform_name, uint32_t array_index, const VknTextureView& texture, bool skip_if_bound, vk::ImageLayout layout)
	{
		auto itr = _uniform_names.find(uniform_name);
		bool bound = false;
		if (itr != _uniform_names.end())
		{
			auto& info = itr->second;
			bound =_bindings.BindAttachment(info, array_index, texture, skip_if_bound, layout);
		}
		return bound;
	}
	hash_table<uint32_t, vector_span<UniformManager::BindingInfo>> UniformManager::FinalizeCurrent()
	{
		hash_table<uint32_t, vector_span<BindingInfo>> result;
		for (auto& [index, set] : _bindings.curr_bindings)
		{
			auto bindings = set.FinalizeDC(_collated_layouts,_buffer_builder);
			if (bindings)
			{
				if(bindings->size())
					result.emplace(index, *bindings);
			}
		}
		return result;
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
	bool UniformUtils::binding_manager::BindUniformBuffer(UniInfo info, uint32_t array_index, vk::Buffer buffer,size_t offset, bool skip_if_bound)
	{
		bool bound = false;

		auto itr2 = curr_bindings.find(info.set);
		if (!skip_if_bound || itr2 == curr_bindings.end() || !detail::is_bound(itr2->second, info.binding, array_index))
		{

			curr_bindings[info.set].Bind(BindingInfo{ info.binding,buffer,offset, array_index, info.size,info.layout });
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
}