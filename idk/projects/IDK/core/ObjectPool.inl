#pragma once
#include "ObjectPool.h"
#include "ObjectHandle.h"

namespace idk
{
	template<typename T>
	ObjectPool<T>::ObjectPool(size_t reserve)
		: lookup(reserve)
	{
		intern.reserve(reserve * sizeof(T));
	}
	
	template<typename T>
	inline T* ObjectPool<T>::at(const Handle& handle) const
	{
		auto& lookup_inflect = lookup[handle.index];
		if (lookup_inflect.uses != handle.uses)
			return nullptr;

		return &data()[lookup_inflect.intern_index];
	}

	template<typename T>
	T* ObjectPool<T>::data() const
	{
		return r_cast<T*>(c_cast<byte*>(intern.data()));
	}

	template<typename T>
	typename ObjectPool<T>::Handle ObjectPool<T>::emplace()
	{
		auto& create_inflect = lookup[first_free];

		auto index = first_free;

		// if we can grab off the freelist, grab
		if (freelist != invalid)
		{
			create_inflect.intern_index = freelist;
			freelist = r_cast<index_t&>(data()[freelist]);
		}
		else
		// else grab it from the end of the intern
		{
			create_inflect.intern_index = in_use++;
		}

		new (&data()[create_inflect.intern_index]) T{};
		++create_inflect.uses;

		// first_free is now invalid
		shift_first_free();
		
		return Handle{ index, create_inflect.uses };
	}

	template<typename T>
	inline void ObjectPool<T>::shift_first_free()
	{
		while (lookup[first_free].intern_index != invalid)
			++first_free;
	}
}