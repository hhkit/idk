#pragma once

namespace idk
{
	template<typename T>
	span<T>::span(T* begin, T* end)
		: _begin{ begin }, _end{ end }
	{}

	template<typename T>
	template<typename Container>
	span<T>::span(Container& c)
		: _begin{ c.begin() }, _ed{ c.end() }
	{
	}

	template<typename T>
	inline T* span<T>::begin() const
	{
		return _begin;
	}

	template<typename T>
	inline T* span<T>::end() const
	{
		return _end;
	}
	template<typename T>
	inline size_t span<T>::size() const
	{
		return _end - _begin;
	}
	template<typename T>
	inline T& span<T>::operator[](size_t index) const
	{
		return _begin[index];
	}
}