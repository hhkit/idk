#pragma once
#include <memory>
#include <cassert>
#include "pool.h"

namespace idk
{
	template<typename T>
	pool<T>::pool(size_t reserve)
		: _intern{}
	{
		_intern.resize(reserve * sizeof(T));
	}
	template<typename T>
	pool<T>::~pool()
	{
		std::destroy(data(), data() + size());
	}
	template<typename T>
	typename pool<T>::iterator_t pool<T>::begin()
	{
		return data();
	}
	template<typename T>
	typename pool<T>::iterator_t pool<T>::end()
	{
		return data() + size();
	}
	template<typename T>
	span<T> pool<T>::span()
	{
		return idk::span<T>(*this);
	}
	template<typename T>
	inline T& pool<T>::front()
	{
		assert(_size);
		return *data();
	}
	template<typename T>
	inline T& pool<T>::back()
	{
		assert(_size);
		return data()[_size - 1];
	}
	template<typename T>
	T* pool<T>::data()
	{
		return r_cast<T*>(_intern.data());
	}
	template<typename T>
	size_t pool<T>::size() const
	{
		return _size;
	}
	template<typename T>
	T& pool<T>::operator[](const size_t& index)
	{
		assert(index < size());
		return data()[index];
	}
	template<typename T>
	void pool<T>::reserve(size_t sz)
	{
		assert(sz > _size);
		vector<byte> new_intern(sz * sizeof(T));
		std::uninitialized_move(begin(), end(), r_cast<T*>(new_intern.data()));
		this->~pool();
		_intern = std::move(new_intern);
	}
	template<typename T>
	void pool<T>::grow()
	{
		reserve(_intern.size() * 2 / sizeof(T));
	}

	template<typename T>
	template<typename ... Args>
	size_t pool<T>::emplace_back(Args&& ... args)
	{
		if (_size *sizeof(T) == _intern.size()) // cap
			grow();

		new (&data()[_size++]) T{ std::forward<Args>(args)... };
		
		return _size - 1;
	}

	template<typename T>
	void pool<T>::pop_back()
	{
		assert(_size != 0);

		std::destroy_at(&data()[--_size]);
	}
}