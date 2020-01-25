#pragma once
#include <vkn/vector_span.h>

namespace idk
{
	template<typename T>
	struct vector_span_builder
	{
		vector_span_builder(vector<T>& vec) : _buffer{ vec }{}

		void start()
		{
			_span.set_buffer(_buffer);
			_span.begin = _buffer.size();
		}

		template<typename ...Args>
		decltype(auto) emplace_back(Args&& ... args)
		{
			return _buffer.emplace_back(std::forward<Args>(args)...);
		}

		vector_span<T> end()
		{
			_span.end = _buffer.size();
			return _span;
		}

	private:
		vector<T>& _buffer;
		vector_span<T> _span = {};
	};
}