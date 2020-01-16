#pragma once
#include "sliding_window.h"

namespace idk
{
	template<typename T, size_t MaxObject>
	sliding_window<T, MaxObject>::sliding_window()
	{
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::reference sliding_window<T, MaxObject>::operator[](size_t index)
	{
		return objects[index - start];
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::const_reference sliding_window<T, MaxObject>::operator[](size_t index) const
	{
		return objects[index - start];
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::reference sliding_window<T, MaxObject>::front()
	{
		return objects.front();
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::const_reference sliding_window<T, MaxObject>::front() const
	{
		return objects.front();
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::reference sliding_window<T, MaxObject>::back()
	{
		return objects[end_index];
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::const_reference sliding_window<T, MaxObject>::back() const
	{
		return objects[end_index];
	}

	template<typename T, size_t MaxObject>
	inline size_t sliding_window<T, MaxObject>::start_index() const
	{
		return _start_index;
	}

	template<typename T, size_t MaxObject>
	inline size_t sliding_window<T, MaxObject>::end_index() const
	{
		return _end_index;
	}

	template<typename T, size_t MaxObject>
	inline bool sliding_window<T, MaxObject>::has_valid_index(size_t index) const
	{
		return index > _start_index && index < _end_index;
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::iterator sliding_window<T, MaxObject>::begin()
	{
		return objects.begin();
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::iterator  sliding_window<T, MaxObject>::end()
	{
		return objects.end();
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::const_iterator sliding_window<T, MaxObject>::begin() const
	{
		return objects.begin();
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::const_iterator sliding_window<T, MaxObject>::end() const
	{
		return objects.end();
	}

	template<typename T, size_t MaxObject>
	inline bool sliding_window<T, MaxObject>::empty() const
	{
		return _start_index != _end_index;
	}

	template<typename T, size_t MaxObject>
	inline bool sliding_window<T, MaxObject>::full() const
	{
		return size() == max_size();
	}

	template<typename T, size_t MaxObject>
	inline size_t sliding_window<T, MaxObject>::size() const
	{
		return _end_index - _start_index;
	}

	template<typename T, size_t MaxObject>
	inline constexpr size_t sliding_window<T, MaxObject>::max_size() noexcept
	{
		return MaxObject;
	}

	template<typename T, size_t MaxObject>
	inline size_t sliding_window<T, MaxObject>::pop_front()
	{
		if (!empty())
		{
			objects.pop_front();
			++_start_index;
		}
		else
			return npos;
	}

	template<typename T, size_t MaxObject>
	template<typename ...Args>
	inline size_t sliding_window<T, MaxObject>::emplace_back(Args&& ...args)
	{
		if (objects.size() == objects.capacity())
			return npos;

		objects.emplace_back(std::forward<Args>(args)...);
		return _end_index++;
	}
	template<typename T, size_t MaxObject>
	template<typename>
	inline size_t sliding_window<T, MaxObject>::push_back(const T& obj)
	{
		return emplace_back(obj);
	}
	template<typename T, size_t MaxObject>
	template<typename>
	inline size_t sliding_window<T, MaxObject>::push_back(T&& obj)
	{
		return emplace_back(std::move(obj));
	}
}