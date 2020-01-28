#pragma once
#include <ds/span.h>
#include <iterator> //std::data
namespace idk
{
	struct index_span
	{
		size_t _begin = 0, _end = 0;
		size_t size()const { return _end - _begin; }
		template<typename T, typename span_t = span<std::remove_reference_t<decltype(*std::data(std::declval<T>()))>>>
		span_t to_span(T& container)const;
	};
	template<typename T>
	using typed_index_span = index_span;
}
