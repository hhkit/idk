#include "circular_buffer.h"
#pragma once

namespace idk
{
	template <typename T, size_t SZ>
	template<typename FwdIt>
	circular_buffer<T, SZ>::circular_buffer(FwdIt start, FwdIt end)
		: circular_buffer{}
	{
		while (start != end)
			emplace_back(*start++);
	}

	template <typename T, size_t SZ>
	circular_buffer<T, SZ>::circular_buffer(std::initializer_list<T> il)
		: circular_buffer{ il.begin(), il.end() }
	{}


	template <typename T, size_t SZ>
	circular_buffer<T, SZ>::circular_buffer(const circular_buffer& rhs)
		: circular_buffer{ rhs.begin(), rhs.end() }
	{}

	template <typename T, size_t SZ>
	auto& circular_buffer<T, SZ>::operator=(const circular_buffer& rhs)
	{
		~circular_buffer();
		for (auto& elem : rhs)
			emplace_back(elem);
		return *this;
	}

	template <typename T, size_t SZ>
	void circular_buffer<T, SZ>::erase(iterator start_erase, iterator end_erase)
	{
		auto real_end = end();
		while (end_erase != real_end)
			* start_erase++ = std::move(*end_erase++);
		_end = std::addressof(*start_erase);
		while (start_erase != real_end)
			std::destroy_at(--real_end);
	}

	template<typename T, size_t SZ>
	inline void circular_buffer<T, SZ>::move_end()
	{
		if (++_end == reinterpret_cast<T*>(_buf + byte_size))
			_end = reinterpret_cast<T*>(_buf);
	}

	template <typename T, size_t SZ>
	template<typename ... Args>
	bool circular_buffer<T, SZ>::emplace_back(Args&& ... args)
	{
		new (_end) T{ std::forward<Args>(args)... };
		move_end();
		if (_end == _start)
		{
			pop_front();
			return false;
		}
		return true;
	}

	template <typename T, size_t SZ>
	bool circular_buffer<T, SZ>::push_back(T&& obj)
	{
		return emplace_back(std::move(obj));
	}

	template <typename T, size_t SZ>
	bool circular_buffer<T, SZ>::push_back(const T& obj)
	{
		return emplace_back(obj);
	}

	template <typename T, size_t SZ>
	void circular_buffer<T, SZ>::pop_front()
	{
		std::destroy_at(_start);
		if (++_start == reinterpret_cast<T*>(_buf + byte_size))
			_start = reinterpret_cast<T*>(_buf);
	}

	template <typename T, size_t SZ>
	auto& circular_buffer<T, SZ>::front()
	{
		return *_start;
	}

	template <typename T, size_t SZ>
	const auto& circular_buffer<T, SZ>::front() const
	{
		return *_start;
	}

	template <typename T, size_t SZ>
	constexpr bool circular_buffer<T, SZ>::empty() const noexcept
	{
		return _start == _end;
	}

	template <typename T, size_t SZ>
	constexpr auto circular_buffer<T, SZ>::size() const noexcept
	{
		return _end > _start ? _end - _start : SZ - (_end - _start + 1);
	}

	template <typename T, size_t SZ>
	auto circular_buffer<T, SZ>::begin()
	{
		return iterator{ this, _start };
	}

	template <typename T, size_t SZ>
	auto circular_buffer<T, SZ>::end()
	{
		return iterator{ this, _end };
	}

	template <typename T, size_t SZ>
	auto circular_buffer<T, SZ>::begin() const
	{
		return const_iterator{ this, _start };
	}

	template <typename T, size_t SZ>
	auto circular_buffer<T, SZ>::end() const
	{
		return const_iterator{ this, _end };
	}

	template <typename T, size_t SZ>
	constexpr auto circular_buffer<T, SZ>::capacity() const noexcept
	{
		return SZ;
	}

	template <typename T, size_t SZ>
	circular_buffer<T, SZ>::~circular_buffer() noexcept
	{
		while (!empty())
			pop_front();
	}

	template<typename T, size_t SZ>
	struct circular_buffer<T, SZ>::iterator
	{
		circular_buffer<T, SZ>* owner;
		T* ptr;

		bool operator!=(const iterator& rhs) const
		{
			return ptr != rhs.ptr;
		}

		bool operator==(const iterator& rhs) const
		{
			return ptr == rhs.ptr;
		}

		iterator& operator++()
		{
			++ptr;
			if (ptr == reinterpret_cast<T*>(owner->_buf + byte_size))
				ptr = reinterpret_cast<T*>(owner->_buf);
			return *this;
		}
		iterator operator++(int)
		{
			iterator result{ *this };
			++(*this);
			return result;
		}
		iterator& operator--()
		{
			if (ptr == reinterpret_cast<T*>(owner->_buf))
				ptr = reinterpret_cast<T*>(owner->_buf + byte_size - sizeof(T));
			else
				--ptr;
			return *this;
		}
		iterator operator--(int)
		{
			iterator result{ *this };
			--(*this);
			return result;
		}

		T& operator*() const
		{
			return *ptr;
		}

		T* operator->() const
		{
			return ptr;
		}
	};

	template<typename T, size_t SZ>
	struct circular_buffer<T, SZ>::const_iterator
	{
		const circular_buffer* owner;
		const T* ptr;

		bool operator!=(const const_iterator& rhs) const
		{
			return ptr != rhs.ptr;
		}

		bool operator==(const const_iterator& rhs) const
		{
			return ptr == rhs.ptr;
		}

		const_iterator& operator++()
		{
			++ptr;
			if (ptr == reinterpret_cast<const T*>(owner->_buf + byte_size))
				ptr = reinterpret_cast<const T*>(owner->_buf);
			return *this;
		}
		auto operator++(int)
		{
			auto result{ *this };
			++(*this);
			return result;
		}
		auto& operator--()
		{
			if (ptr == reinterpret_cast<const T*>(owner->_buf))
				ptr = reinterpret_cast<const T*>(owner->_buf + byte_size - sizeof(T));
			else
				--ptr;
			return *this;
		}
		auto operator--(int)
		{
			auto result{ *this };
			--(*this);
			return result;
		}

		auto& operator*() const
		{
			return *ptr;
		}

		auto* operator->() const
		{
			return ptr;
		}
	};
}
