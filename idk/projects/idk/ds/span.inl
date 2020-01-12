#pragma once
#include <utility>
#include "span.h"

namespace idk
{
	namespace detail
	{
	}

	template<typename T>
	constexpr span<T>::span(T* begin, T* end) noexcept
		: _begin{ begin }, _end{ end }
	{}

	template<typename T>
	template<typename Container, typename>
	constexpr span<T>::span(Container&& c) noexcept
		: _begin{ std::data(c) }, _end{ std::data(c) + std::size(c) }
	{
	}

	template<typename T>
	constexpr T* span<T>::begin() const noexcept
	{
		return _begin;
	}

	template<typename T>
	constexpr T* span<T>::end() const noexcept
	{
		return _end;
	}
	template<typename T>
	inline constexpr span<T> span<T>::subspan(size_t offset, size_t count) const noexcept
	{
		return span<T>{_begin + offset, count != npos ? _begin + offset + count : _end};
	}
	template<typename T>
	inline constexpr T* span<T>::data() const noexcept
	{
		return _begin;
	}
	template<typename T>
	constexpr size_t span<T>::size() const noexcept 
	{
		return _end - _begin;
	}

	template<typename T>
	inline constexpr size_t span<T>::size_bytes() const noexcept
	{
		return size() * sizeof(T);
	}

	template<typename T>
	inline constexpr bool span<T>::empty() const noexcept
	{
		return _begin == _end;
	}

	template<typename T>
	constexpr T& span<T>::operator[](size_t index) const noexcept
	{
		return _begin[index];
	}
	template<typename T>
	inline constexpr T& span<T>::front() const noexcept
	{
		return *_begin;
	}
	template<typename T>
	inline constexpr T& span<T>::back() const noexcept
	{
		return _end[-1];
	}
}