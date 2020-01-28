#pragma once
#include <idk.h>
#include <ds/index_span.inl>
namespace idk
{
	template<typename T,typename vector_t = vector<T> >
	struct vector_span : index_span
	{
		using span_t = span<T>;
		using index_span::to_span;
		std::remove_reference_t<T>* begin()const { return _buffer->data() + index_span::_begin; }
		std::remove_reference_t<T>* end  ()const { return _buffer->data() + index_span::_end; }
		span_t to_span()const;
		void set_buffer(vector_t& buffer) { _buffer = &buffer; }
	private:
		vector_t* _buffer = {};
	};


	template<typename T,typename vector_t>
	inline typename vector_span<T,vector_t>::span_t vector_span<T,vector_t>::to_span() const {
		auto& buffer = *_buffer;
		return span_t{ buffer.data() + _begin,buffer.data() + _end };
	}
}