#pragma once

#include <memory>
#include <ds/compressed_pair.h>

namespace idk
{
	template<typename CharT, typename Allocator = std::allocator<CharT>>
	class alignas(32) small_string
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
		using iterator = CharT*;
		using const_iterator = const CharT*;

		explicit small_string(const Allocator& alloc) noexcept;
		small_string() noexcept(noexcept(Allocator));
		small_string(const CharT* cstr, const Allocator& alloc = Allocator{});
		template<typename InputIt> small_string(InputIt first, InputIt last, const Allocator& alloc = Allocator());

		~small_string();

		small_string(const small_string&);
		small_string(small_string&&);
		small_string& operator=(const small_string&);
		small_string& operator=(small_string&&);

		// size / capacity accessors
		size_type size() const;
		size_type length() const;
		bool empty() const;
		size_type capacity() const;

		// data accessors
		reference at(size_type pos);
		const_reference at(size_type pos) const;
		reference front();
		const_reference front() const;
		reference back();
		const_reference back() const;
		const CharT* data() const;
		CharT* data();
		const CharT* c_str() const;

		// iterators
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;

		// modifiers
		void clear();
		void reserve(size_type new_cap);
		void push_back(CharT c);

	private:
		constexpr static CharT _sso_buffer_size = (31 / sizeof(CharT));
		constexpr static CharT _use_longer_mask = 0x01;

		struct _sso
		{
			CharT buffer[_sso_buffer_size]; // buf size should be > 24
			CharT size_diff;
		};
		struct _longer
		{
			CharT* ptr;
			size_type size;
			size_type capacity;
		};
		union _rep
		{
			_sso sso;
			_longer longer;
		};

		bool _is_sso() const;
		size_type _calc_capacity(size_type len) const;

		// compress allocator, since it's most likely size 0
		compressed_pair<_rep, allocator_type> _rep;
	};
}

#include "small_string.inl"