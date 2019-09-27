#include "ResourceBundle.h"
#include <res/ResourceUtils.h>
#pragma once

namespace idk
{
	template<typename Res>
	ResourceBundle::ResourceBundle(const RscHandle<Res>& handle)
	{
		Add(handle);
	}
	template<typename T>
	RscHandle<T> ResourceBundle::Get() const
	{
		auto& subarray = subarrays[BaseResourceID<T>];
		return subarray.count > 0 ? handles[subarray.index].AsHandle<T>() : RscHandle<T>();
	}

	template<typename T>
	ResourceBundle::ResourceSpan<T> ResourceBundle::GetAll() const
	{
		auto& subarray = subarrays[BaseResourceID<T>];
		return ResourceSpan{ handles.data() + subarray.index, handles.data() + subarray.index + subarray.count };
	}

	template<typename T>
	inline void ResourceBundle::Add(RscHandle<T> handle)
	{
		auto& sub_arr = subarrays[BaseResourceID<T>];
		auto new_ind = sub_arr.index + sub_arr.count++;

		
		// make space for new resource
		handles.emplace_back();
		std::move_backward(handles.data() + new_ind, handles.data() + handles.size() - 1, handles.data() + handles.size() );

		// assign new resource
		handles[new_ind] = handle;

		// push back all subsequent resources
		for (auto& elem : span<sub_array>{ &sub_arr + 1, subarrays.data() + subarrays.size() })
			++elem.index;
	}

	template<typename T>
	ResourceBundle::ResourceSpan<T>::ResourceSpan(GenericResourceHandle* btr, GenericResourceHandle* etr)
		: span_over{btr, etr}
	{
	}

	template<typename T>
	typename ResourceBundle::ResourceSpan<T>::iterator ResourceBundle::ResourceSpan<T>::begin() const
	{
		return iterator{ span_over.begin() };
	}

	template<typename T>
	typename ResourceBundle::ResourceSpan<T>::iterator ResourceBundle::ResourceSpan<T>::end() const
	{
		return iterator{ span_over.end() };
	}

	template<typename T>
	inline size_t ResourceBundle::ResourceSpan<T>::size() const
	{
		return span_over.size();
	}

	template<typename T>
	ResourceBundle::ResourceSpan<T>::iterator::iterator(GenericResourceHandle* h)
		: itr{ h }
	{
	}

	template<typename T>
	T& ResourceBundle::ResourceSpan<T>::iterator::operator*() const
	{
		return itr->AsHandle<T>();
	}

	template<typename T>
	T* ResourceBundle::ResourceSpan<T>::iterator::operator->() const
	{
		return &itr->AsHandle<T>();
	}

	template<typename T>
	typename ResourceBundle::ResourceSpan<T>::iterator& ResourceBundle::ResourceSpan<T>::iterator::operator++()
	{
		++itr;
		return *this;
	}

	template<typename T>
	typename ResourceBundle::ResourceSpan<T>::iterator  ResourceBundle::ResourceSpan<T>::iterator::operator++(int)
	{
		auto copy = *this;
		++itr;
		return copy;
	}

	template<typename T>
	bool ResourceBundle::ResourceSpan<T>::iterator::operator<(const iterator& rhs) const
	{
		return itr < rhs.itr;
	}
}