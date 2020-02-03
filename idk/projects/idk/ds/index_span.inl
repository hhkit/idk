#pragma once
#include "index_span.h"
namespace idk
{
	template<typename T, typename span_t>
	span_t index_span::to_span(T& container)const
	{
		return span_t{ std::data(container) + _begin ,std::data(container) + _end };
	}
}