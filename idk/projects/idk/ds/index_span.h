#pragma once
#include <ds/span.h>
#include <iterator> //std::data
namespace idk
{
	struct index_span
	{
		size_t begin = 0, end = 0;
		size_t size()const { return end - begin; }
		template<typename T, typename span_t = span<std::remove_reference_t<decltype(*std::data(std::declval<T>()))>>>
		span_t to_span(T& container)const;
	};

}
