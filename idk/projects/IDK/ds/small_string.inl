#pragma once

#include "small_string.h"

namespace idk
{

	template<typename CharT, typename Allocator>
	constexpr small_string<CharT, Allocator>::small_string(const Allocator& alloc) noexcept
		: _rep{ rep{rep::sso{{}, _buffer_size}}, alloc }
	{
		static_assert(sizeof(CharT) <= 8);
	}

	template<typename CharT, typename Allocator>
	constexpr small_string<CharT, Allocator>::small_string() noexcept(noexcept(Allocator))
		: small_string(Allocator{})
	{
	}

	template<typename CharT, typename Allocator>
	constexpr small_string<CharT, Allocator>::small_string(const CharT* cstr, const Allocator& alloc)
		: small_string(alloc)
	{

	}

}