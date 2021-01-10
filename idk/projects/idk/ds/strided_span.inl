#pragma once
#include <utility>
#include "strided_span.h"

namespace idk
{
	namespace detail
	{
	}

	template<typename T>
	constexpr strided_span<T>::strided_span(T* begin, T* end,size_t offset) noexcept
		: _begin{ begin }, _end{ end },_offset{offset}
	{}

	template<typename T>
	template<typename Derived, typename>
	inline constexpr strided_span<T>::strided_span(Derived* begin, Derived* end, size_t offset) noexcept
		: _begin{ begin }, _end{ end }, _offset{ offset+sizeof(Derived)-sizeof(T) }
	{
	}

	template<typename T>
	template<typename Container, typename>
	constexpr strided_span<T>::strided_span(Container&& c) noexcept
		: _begin{ std::data(c) }, _end{ std::data(c) + std::size(c) },_offset{ sizeof(*std::data(c))-sizeof(T)}
	{
	}

	template<typename T>
	constexpr typename strided_span<T>::iterator strided_span<T>::begin() const noexcept
	{
		return iterator{ _begin,_offset };
	}

	template<typename T>
	constexpr typename strided_span<T>::iterator strided_span<T>::end() const noexcept
	{
		return iterator{ _end,_offset };
	}
	template<typename T>
	inline constexpr strided_span<T> strided_span<T>::subspan(size_t offset, size_t count) const noexcept
	{
		return strided_span<T>{_begin + offset, count != npos ? _begin + offset + count : _end};
	}
	template<typename T>
	inline constexpr T* strided_span<T>::data() const noexcept
	{
		return _begin;
	}
	template<typename T>
	constexpr size_t strided_span<T>::size() const noexcept
	{
		return;// _end - _begin;
	}

	template<typename T>
	inline constexpr size_t strided_span<T>::size_bytes() const noexcept
	{
		return size() * (sizeof(T)+_offset);
	}

	template<typename T>
	inline constexpr bool strided_span<T>::empty() const noexcept
	{
		return _begin == _end;
	}

	template<typename T>
	inline constexpr typename strided_span<T>::iterator strided_span<T>::get_iterator(T* ptr) const noexcept
	{
		return iterator{ ptr,_offset };
	}

	template<typename T>
	constexpr T& strided_span<T>::operator[](size_t index) const noexcept
	{
		return *(begin()+index);
	}
	template<typename T>
	inline constexpr T& strided_span<T>::front() const noexcept
	{
		return *_begin;
	}
	template<typename T>
	inline constexpr T& strided_span<T>::back() const noexcept
	{
		return *(end() -1);
	}
}