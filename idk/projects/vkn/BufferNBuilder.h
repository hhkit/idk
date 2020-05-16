#pragma once
#include <idk.h>
#include <vkn/vector_span_builder.h>
namespace idk::vkn
{
	template<typename T, template <typename> typename vec = vector>
	struct BufferNBuilder
	{
		vec<T> buffer;
		vector_span_builder<T> builder{ buffer };
		BufferNBuilder() = default;
		BufferNBuilder(BufferNBuilder&& rhs)noexcept : buffer{ std::move(rhs.buffer) }, builder{ std::move(rhs.builder) }
		{
			builder.set(buffer);
		}
		BufferNBuilder& operator=(BufferNBuilder&& rhs)noexcept
		{
			buffer = std::move(rhs.buffer);
			builder = std::move(rhs.builder);
			builder.set(buffer);
			return *this;
		}
	};
}