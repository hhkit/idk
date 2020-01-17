#pragma once
#include <array>
#include <memory>
#include "circular_buffer.h"

namespace idk
{
	template<typename T, size_t MaxObject>
	class sliding_window
	{
		using underlying_container = std::aligned_storage_t<sizeof(T) * MaxObject * 2, alignof(T)>;
	public:
		using key_type = unsigned short;
		using mapped_type = T;
		struct iter_type;
		struct const_iter_type;
		using size_t = unsigned short;
		using difference_type = std::ptrdiff_t;
		struct iterator;
		struct const_iterator;

		static constexpr auto npos = static_cast<size_t>(-1);

		sliding_window();

		// element access
		T&       operator[](size_t index);
		const T& operator[](size_t index) const;
		T&       front();
		const T& front() const;
		T&       back();
		const T& back() const;

		// window management
		size_t start_index() const;
		size_t end_index() const;
		bool   contains(size_t index) const;

		// iterators
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;

		// capacity
		bool empty() const;
		bool full() const;
		size_t size() const;
		constexpr size_t max_size() const noexcept;
		constexpr size_t capacity() const noexcept;

		// modifiers
		// will return the index of the emplaced element or npos if it fails due to full buffer
		template<typename ... Args>
		size_t emplace_back(Args&& ... args);
		template<typename = std::enable_if_t<std::is_copy_constructible_v<T>>>
		size_t push_back(const T& obj);
		template<typename = std::enable_if_t<std::is_move_constructible_v<T>>>
		size_t push_back(T&& obj);
		size_t pop_front();

		// lookup
		iterator find(size_t index);
		const_iterator find(size_t index) const;
	private:
		size_t _start_index{}, _end_index{};
		union
		{
			underlying_container data;
			T objects[1];
		};

		static auto sequence_greater_than(short lhs, short rhs);
	};
}