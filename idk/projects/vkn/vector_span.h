#pragma once
#include <idk.h>
#include <ds/index_span.h>
namespace idk
{
	template<typename T,typename vector_t = vector<T> >
	struct vector_span : index_span
	{
		using span_t = span<T>;
		using index_span::to_span;
		span_t to_span()const;
		void set_buffer(vector_t& buffer) { _buffer = &buffer; }
	private:
		vector_t* _buffer = {};
	};


	template<typename T,typename vector_t>
	inline typename vector_span<T,vector_t>::span_t vector_span<T,vector_t>::to_span() const {
		auto& buffer = *_buffer;
		return span_t{ buffer.data() + begin,buffer.data() + end };
	}
}