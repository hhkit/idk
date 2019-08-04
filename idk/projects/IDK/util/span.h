#pragma once
namespace idk
{
	template <typename T>
	struct span
	{
		T* _begin;
		T* _end;

		span(T* begin, T* end);
		template <typename Container>
		explicit span(Container& c);
		T* begin() const;
		T* end() const;
		size_t size() const;
		T& operator[](size_t index) const;
	};
}

#include "span.inl"