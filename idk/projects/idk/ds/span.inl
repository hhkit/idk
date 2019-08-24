#pragma once

namespace idk
{
	template<typename T>
	constexpr span<T>::span(T* begin, T* end) noexcept
		: _begin{ begin }, _end{ end }
	{}

	template<typename T>
	template<typename Container>
	constexpr span<T>::span(Container& c) noexcept
		: _begin{ c.begin() }, _end{ c.end() }
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
	constexpr size_t span<T>::size() const noexcept 
	{
		return _end - _begin;
	}
	template<typename T>
	constexpr T& span<T>::operator[](size_t index) const noexcept
	{
		return _begin[index];
	}
}