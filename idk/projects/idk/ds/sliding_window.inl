#pragma once
#include "sliding_window.h"
#include <stdexcept>

namespace idk
{

	template<typename T, size_t MaxObject>
	struct sliding_window<T, MaxObject>::iter_type
	{
		const key_type key;
		mapped_type& value;

		iter_type(key_type k, mapped_type& m)
			: key{ k }, value{ m }
		{}
	};

	template<typename T, size_t MaxObject>
	struct sliding_window<T, MaxObject>::const_iter_type
	{
		const key_type key;
		const mapped_type& value;

		const_iter_type(key_type k, const mapped_type& m)
			: key{ k }, value{ m }
		{}
	};

	template<typename T, size_t MaxObject>
	struct sliding_window<T, MaxObject>::iterator
	{
		T* iterator;
		T* start;
		T* end;

		typename sliding_window<T, MaxObject>::iter_type operator*() const
		{
			return iter_type{ static_cast<key_type>(iterator - start), *iterator };
		}

		typename sliding_window<T, MaxObject>::iterator& operator++()
		{
			++iterator;
			if (iterator == end)
				iterator = start;
			return *this;
		}

		typename sliding_window<T, MaxObject>::iterator operator++(int)
		{
			auto copy = *this;
			++(*this);
			return copy;
		}

		bool operator!=(const typename sliding_window<T, MaxObject>::iterator& rhs) const
		{
			return iterator != rhs.iterator;
		}

		bool operator==(const typename sliding_window<T, MaxObject>::iterator& rhs) const
		{
			return iterator == rhs.iterator;
		}
	};

	template<typename T, size_t MaxObject>
	struct sliding_window<T, MaxObject>::const_iterator
	{
		const T* iterator;
		const T* start;
		const T* end;

		typename sliding_window<T, MaxObject>::const_iter_type operator*() const
		{
			return const_iter_type{ static_cast<key_type>(iterator - start),* iterator };
		}

		typename sliding_window<T, MaxObject>::const_iterator& operator++()
		{
			++iterator;
			if (iterator == end)
				iterator = start;
			return *this;
		}

		typename sliding_window<T, MaxObject>::const_iterator operator++(int)
		{
			auto copy = *this;
			++(*this);
			return copy;
		}

		bool operator!=(const typename sliding_window<T, MaxObject>::const_iterator& rhs) const
		{
			return iterator != rhs.iterator;
		}

		bool operator==(const typename sliding_window<T, MaxObject>::const_iterator& rhs) const
		{
			return iterator == rhs.iterator;
		}
	};

	template<typename T, size_t MaxObject>
	sliding_window<T, MaxObject>::sliding_window()
	{
	}

	template<typename T, size_t MaxObject>
	inline sliding_window<T, MaxObject>::~sliding_window() noexcept(std::is_nothrow_destructible_v<T>)
	{
		if (_start_index < _end_index)
			std::destroy(&objects[_start_index], &objects[_end_index]);
		else
		{
			std::destroy(&objects[0], &objects[_end_index]);
			std::destroy(&objects[_start_index], &objects[capacity()]);
		}
	}

	template<typename T, size_t MaxObject>
	T& sliding_window<T, MaxObject>::operator[](size_t index)
	{
		if (!contains(index))
			throw std::out_of_range{"index out of valid range"};	
		return objects[index];
	}

	template<typename T, size_t MaxObject>
	const T& sliding_window<T, MaxObject>::operator[](size_t index) const
	{
		if (!contains(index))
			throw std::out_of_range{ "index out of valid range" };
		return objects[index];
	}

	template<typename T, size_t MaxObject>
	T& sliding_window<T, MaxObject>::front()
	{
		return objects[start_index];
	}

	template<typename T, size_t MaxObject>
	const T& sliding_window<T, MaxObject>::front() const
	{
		return objects[start_index];
	}

	template<typename T, size_t MaxObject>
	T& sliding_window<T, MaxObject>::back()
	{
		return objects[end_index ? end_index-1 : capacity() - 1];
	}

	template<typename T, size_t MaxObject>
	const T& sliding_window<T, MaxObject>::back() const
	{
		return objects[end_index ? end_index - 1 : capacity() - 1];
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::size_t sliding_window<T, MaxObject>::start_index() const
	{
		return _start_index;
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::size_t sliding_window<T, MaxObject>::end_index() const
	{
		return _end_index;
	}

	template<typename T, size_t MaxObject>
	inline bool sliding_window<T, MaxObject>::contains(size_t index) const
	{
		return _start_index < _end_index 
			? (_start_index <= index && index < _end_index) 
			: (_start_index <= index || index < _end_index);
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::iterator sliding_window<T, MaxObject>::begin()
	{
		return iterator{ &objects[_start_index], &objects[0], &objects[capacity()] };
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::iterator  sliding_window<T, MaxObject>::end()
	{
		return iterator{ &objects[_end_index], &objects[0], &objects[capacity()] };
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::const_iterator sliding_window<T, MaxObject>::begin() const
	{
		return const_iterator{ &objects[_start_index], &objects[0], &objects[capacity()] };
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::const_iterator sliding_window<T, MaxObject>::end() const
	{
		return const_iterator{ &objects[_end_index], &objects[0], &objects[capacity()] };
	}

	template<typename T, size_t MaxObject>
	inline bool sliding_window<T, MaxObject>::empty() const
	{
		return _start_index == _end_index;
	}

	template<typename T, size_t MaxObject>
	inline bool sliding_window<T, MaxObject>::full() const
	{
		return size() == max_size();
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::size_t sliding_window<T, MaxObject>::size() const
	{
		return _end_index > _start_index ? _end_index - _start_index : capacity() - _start_index + _end_index;
	}

	template<typename T, size_t MaxObject>
	constexpr typename sliding_window<T, MaxObject>::size_t sliding_window<T, MaxObject>::max_size() const noexcept
	{
		return MaxObject;
	}

	template<typename T, size_t MaxObject>
	constexpr typename sliding_window<T, MaxObject>::size_t sliding_window<T, MaxObject>::capacity() const noexcept
	{
		return MaxObject * 2;
	}


	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::size_t sliding_window<T, MaxObject>::pop_front()
	{
		if (!empty())
		{
			std::destroy_at(&objects[_start_index]);
			if (++_start_index == capacity())
				_start_index = 0;
			return 0;
		}
		else
			return npos;
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::iterator sliding_window<T, MaxObject>::find(size_t index)
	{
		if (contains(index))
			return iterator{ &objects[index], &objects[0], &objects[capacity()] };
		return end();
	}

	template<typename T, size_t MaxObject>
	typename sliding_window<T, MaxObject>::const_iterator sliding_window<T, MaxObject>::find(size_t index) const
	{
		if (contains(index))
			return const_iterator{ &objects[index], &objects[0], &objects[capacity()] };
		return end();
	}


	template<typename T, size_t MaxObject>
	template<typename ...Args>
	typename sliding_window<T, MaxObject>::size_t sliding_window<T, MaxObject>::emplace_back(Args&& ...args)
	{
		if (size() == max_size())
			return npos;

		new (&objects[_end_index++]) T(std::forward<Args>(args)...);
		if (_end_index == capacity())
			_end_index = 0;
		return _end_index;
	}
	template<typename T, size_t MaxObject>
	template<typename>
	typename sliding_window<T, MaxObject>::size_t sliding_window<T, MaxObject>::push_back(const T& obj)
	{
		return emplace_back(obj);
	}
	template<typename T, size_t MaxObject>
	template<typename>
	typename sliding_window<T, MaxObject>::size_t sliding_window<T, MaxObject>::push_back(T&& obj)
	{
		return emplace_back(std::move(obj));
	}

	template<typename T, size_t MaxObject>
	inline auto sliding_window<T, MaxObject>::sequence_greater_than(short lhs, short rhs)
	{
		return ((lhs > rhs) && (lhs - rhs <= MaxObject)) ||
			((lhs < rhs) && (rhs - lhs <= MaxObject));
	}
}