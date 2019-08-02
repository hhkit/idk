#pragma once
#include "ObjectPool.h"
#include "ObjectHandle.h"
#include <memory>
#include <cmath>

namespace idk
{
	template<typename T>
	ObjectPool<T>::ObjectPool(size_t reserve)
		: lookup(reserve), intern(reserve*sizeof(T))
	{
	}

	template<typename T>
	inline ObjectPool<T>::~ObjectPool()
	{
		std::destroy(data(), data() + pool_size);
	}
	
	template<typename T>
	T* ObjectPool<T>::at(const Handle& handle) const
	{
		if (validate(handle) == false)
			return nullptr;

		auto& lookup_inflect = lookup[handle.index];
		return &data()[lookup_inflect.intern_index];
	}

	template<typename T>
	T* ObjectPool<T>::data() const
	{
		return r_cast<T*>(c_cast<byte*>(intern.data()));
	}

	template<typename T>
	inline bool ObjectPool<T>::validate(const Handle& handle) const
	{
		if (handle.index > lookup.size())
			return false;

		return lookup[handle.index].intern_index != invalid && lookup[handle.index].uses == handle.uses;
	}

	template<typename T>
	typename ObjectPool<T>::Handle ObjectPool<T>::emplace()
	{
		if (first_free_handle == lookup.size())
			grow();

		auto& create_inflect = lookup[first_free_handle];

		auto index = first_free_handle;

		auto res = create();
		create_inflect.intern_index = std::get<index_t>(res);
		
		++create_inflect.uses;

		// first_free is now invalid
		shift_first_free();
		
		return std::get<T&>(res).handle = Handle{ index, create_inflect.uses };
	}

	template<typename T>
	typename ObjectPool<T>::Handle ObjectPool<T>::emplace_at(const Handle& handle)
	{
		if (handle.index == first_free_handle)
			return emplace();

		if (handle.index > lookup.size())
			grow();

		auto& create_inflect = lookup[handle.index];

		// ensure that the slot is unused
		assert(lookup[handle.index].intern_index != invalid);

		auto res = create();
		create_inflect.intern_index = std::get<index_t>(res);
		create_inflect.uses         = handle.uses;

		return std::get<T&>(res).handle = Handle{ index, create_inflect.uses };
	}

	template<typename T>
	inline bool ObjectPool<T>::remove(const Handle& deleteHandle)
	{
		if (validate(deleteHandle) == false)
			return false;

		auto& deleteme = data()[lookup[deleteHandle.index].intern_index];
		auto& end = data()[--pool_size];

		auto endHandle = end.GetHandle();

		std::swap(lookup[deleteHandle.index], lookup[endHandle.index]);
		std::swap(deleteme, end);

		lookup[deleteHandle.index].intern_index = invalid;
		std::destroy_at(&end);

		if (deleteHandle.index < first_free_handle)
			first_free_handle = deleteHandle.index;

		return true;
	}

	template<typename T>
	inline bool ObjectPool<T>::remove(T& removeme)
	{
		return remove(removeme.GetHandle());
	}

	template<typename T>
	T& ObjectPool<T>::operator[](const Handle& handle) const
	{
		return *at(handle);
	}

	template<typename T>
	void ObjectPool<T>::shift_first_free()
	{
		while (first_free_handle != lookup.size() && lookup[first_free_handle].intern_index != invalid)
			++first_free_handle;
	}
	template<typename T>
	void ObjectPool<T>::grow()
	{
		lookup.resize(lookup.size() * 2);

		auto new_intern = vector<byte>(intern.size() * 2);
		
		auto internal_start = r_cast<T*>(intern.data());
		auto internal_end   = internal_start + pool_size;
		auto dest           = r_cast<T*>(new_intern.data());
		std::uninitialized_move(internal_start, internal_end, dest);
		std::destroy(internal_start, internal_end);

		intern = std::move(new_intern);
	}
	template<typename T>
	tuple<T&, typename ObjectPool<T>::index_t> ObjectPool<T>::create()
	{
		index_t retval = pool_size++;
		return tuple<T&, index_t>{ *new (&data()[retval]) T{}, retval};
	}
}