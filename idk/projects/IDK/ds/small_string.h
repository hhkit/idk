#pragma once

#include <memory>
#include <ds/compressed_pair.h>

namespace idk
{
	template<typename CharT, typename Allocator = std::allocator<CharT>>
	class small_string
	{
	public:
		using value_type = CharT;
		using allocator_type = Allocator;
		using size_type = typename std::allocator_traits<Allocator>::size_type;
		using difference_type = typename std::allocator_traits<Allocator>::difference_type;
		using reference	= value_type&;
		using const_reference = const value_type&;
		using pointer = typename std::allocator_traits<Allocator>::pointer;
		using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;

		constexpr explicit small_string(const Allocator& alloc) noexcept;
		constexpr small_string() noexcept(noexcept(Allocator));
		constexpr small_string(const CharT* cstr, const Allocator& alloc = Allocator{});

	private:
		constexpr static size_t _buffer_size = (31 / sizeof(CharT));
		constexpr static unsigned char _sso_mask = 0x01;

		union rep
		{
			struct sso
			{
				CharT buffer[_buffer_size]; // buf size should be > 24
				unsigned char size;
			} _sso;
			struct allocated
			{
				CharT* ptr;
				size_t size;
				size_t capacity;
			} _alloc;
		};

		bool _is_sso()
		{
			return _sso.size & _sso_mask;
		}

		// compress allocator, since it's most likely size 0
		compressed_pair<rep, allocator_type> _rep;
	};
}

#include "small_string.inl"