#pragma once
#include "index_span.h"
namespace idk
{
	template<typename T, typename span_t>
	span_t index_span::to_span(T& container)
	{
		return span_t{ std::data(container) + begin ,std::data(container) + end };
	}
}