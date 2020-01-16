#pragma once
#include "circular_buffer.h"

namespace idk
{
	template<typename T, size_t MaxObject>
	class sliding_window
	{
		using underlying_data = circular_buffer<T, MaxObject>;
	public:
		using element_type = T;
		using value_type = std::remove_cv_t<T>;
		using size_t = std::size_t;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using typename underlying_data::iterator;
		using typename underlying_data::const_iterator;

		static constexpr auto npos = static_cast<size_t>(-1);

		sliding_window();

		// element access
		reference       operator[](size_t index);
		const_reference operator[](size_t index) const;
		reference       front();
		const_reference front() const;
		reference       back();
		const_reference back() const;

		// window management
		size_t start_index() const;
		size_t end_index() const;
		bool   has_valid_index(size_t index) const;

		// iterators
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;

		// capacity
		bool empty() const;
		bool full() const;
		size_t size() const;
		constexpr size_t max_size() noexcept;

		// modifiers
		// will return the index of the emplaced element or npos if it fails due to full buffer
		template<typename ... Args>
		size_t emplace_back(Args&& ... args);
		template<typename = std::enable_if_t<std::is_copy_constructible_v<T>>>
		size_t push_back(const T& obj);
		template<typename = std::enable_if_t<std::is_move_constructible_v<T>>>
		size_t push_back(T&& obj);

		size_t pop_front();
	private:
		size_t _start_index{}, _end_index{};
		underlying_data objects;
	};
}