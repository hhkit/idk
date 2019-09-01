/******************************************************************************/
/*!
\project	September
\file		CircularBuffer.h
\author		Ho Han Kit Ivan
\brief
A circular buffer that is allocated once and you never have to touch it again.

All content (C) 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once
#include <initializer_list>
#include <algorithm>
#include <memory>

namespace idk
{
	template <typename T, size_t SZ>
	class circular_buffer
	{
	public:
		struct iterator;
		struct const_iterator;

		// constructors
		constexpr circular_buffer() = default;

		template<typename FwdIt>
		circular_buffer(FwdIt start, FwdIt end);
		circular_buffer(std::initializer_list<T> il);

		circular_buffer(const circular_buffer& rhs);
		auto& operator=(const circular_buffer& rhs);
		~circular_buffer() noexcept;

		// accessors
		constexpr bool empty() const noexcept;
		constexpr auto size() const noexcept;
		constexpr auto capacity() const noexcept;

		// iterators
		auto begin();
		auto end();
		auto begin() const;
		auto end() const;

		auto& front();
		const auto& front() const;

		// modifiers
		bool push_back(T&& obj);
		bool push_back(const T& obj);
		void pop_front();

		void erase(iterator start_erase, iterator end_erase);

		template<typename ... Args>
		bool emplace_back(Args&& ... args);
	private:
		static constexpr size_t byte_size = (SZ + 1) * sizeof(T);
		alignas(alignof(T)) char _buf[byte_size]{};
		T* _start = reinterpret_cast<T*>(_buf);
		T* _end = reinterpret_cast<T*>(_buf);

		void move_end();
	};
}
#include "circular_buffer.inl"