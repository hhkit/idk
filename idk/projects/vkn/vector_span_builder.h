#pragma once
#include <vkn/vector_span.h>

namespace idk
{
	template<typename T>
	struct vector_span_builder
	{
		vector_span_builder(vector<T>& vec) : _buffer{ &vec }{}

		void set(vector<T>& vec)
		{
			_buffer = &vec;
		}

		void grow_reserve(size_t additional_size)
		{
			auto new_size = additional_size + _buffer->size();
			if(new_size > _buffer->capacity())
				_buffer->reserve(std::max(new_size, _buffer->capacity() * 2));
		}

		void start()
		{
			_span.set_buffer(*_buffer);
			_span._begin = _buffer->size();
		}

		template<typename ...Args>
		decltype(auto) emplace_back(Args&& ... args)
		{
			return _buffer->emplace_back(std::forward<Args>(args)...);
		}

		vector_span<T> end()
		{
			_span._end = _buffer->size();
			return _span;
		}

	private:
		vector<T>* _buffer;
		vector_span<T> _span = {};
	};
}