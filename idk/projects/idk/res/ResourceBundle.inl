#include "ResourceBundle.h"
#pragma once

namespace idk
{
	template<typename T>
	RscHandle<T> ResourceBundle::Get() const
	{
		auto& subarray = subarrays[ResourceID<typename T::BaseResource>];
		return subarray.count > 0 ? handles[subarray.index].AsHandle<T>() : RscHandle<T>();
	}

	template<typename T>
	ResourceBundle::ResourceSpan<T> ResourceBundle::GetAll() const
	{
		auto& subarray = subarrays[ResourceID<typename T::BaseResource>];
		return ResourceSpan{ handles.data() + subarray.index, handles.data() + subarray.index + subarray.count };
	}

	template<typename T>
	inline void ResourceBundle::Add(RscHandle<T> handle) const
	{
		auto& sub_arr = subarrays[ResourceID<typename T::BaseResource>];
		auto new_ind = sub_arr.index + ++sub_arr.count;

		// make space for new resource
		std::move_backward(handles.data() + new_ind, handles.data() + handles.size() - 1, handles.data() + handles.size());

		// assign new resource
		handles[new_ind] = handle;

		// push back all subsequent resources
		for (auto& elem : span<sub_array>{ &sub_arr, std::end(subarrays) })
			++elem.index;
	}

	template<typename T>
	ResourceBundle::ResourceSpan<T>::ResourceSpan(GenericResourceHandle* btr, GenericResourceHandle* etr)
		: span_over{btr, etr}
	{
	}

	template<typename T>
	ResourceBundle::ResourceSpan<T>::iterator ResourceBundle::ResourceSpan<T>::begin()
	{
		return iterator{ span_over.begin() };
	}

	template<typename T>
	ResourceBundle::ResourceSpan<T>::iterator ResourceBundle::ResourceSpan<T>::end()
	{
		return iterator{ span_over.end() };
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
	ResourceBundle::ResourceSpan<T>::iterator& ResourceBundle::ResourceSpan<T>::iterator::operator++()
	{
		++itr;
		return *this;
	}

	template<typename T>
	ResourceBundle::ResourceSpan<T>::iterator  ResourceBundle::ResourceSpan<T>::iterator::operator++(int)
	{
		auto copy = *this;
		++itr;
		return copy;
	}
}