#pragma once

#include <memory>
#include <ds/compressed_pair.h>
#include <util/string_hash.h>

namespace std // forward decl
{
	template<typename CharT, typename Traits = std::char_traits<CharT>>
	class basic_string_view;
}

namespace idk
{
	template<typename CharT,
		     typename Traits = std::char_traits<CharT>,
		     typename Allocator = std::allocator<CharT>>
	class alignas(32) small_string
	{
	public:
		using traits_type = Traits;
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

		constexpr static auto npos = static_cast<size_type>(-1);

		explicit small_string(const Allocator& alloc) noexcept;
		small_string() noexcept(noexcept(Allocator));
		small_string(size_type count, CharT c, const Allocator& alloc = Allocator{});
		small_string(const CharT* cstr, const Allocator& alloc = Allocator{});
		template<typename InputIt> small_string(InputIt first, InputIt last, const Allocator& alloc = Allocator());

		~small_string();

		small_string(const small_string&);
		small_string(small_string&&);
		small_string& operator=(const small_string&);
		small_string& operator=(small_string&&);

		allocator_type get_allocator() const;

		// size / capacity accessors
		size_type size() const;
		size_type length() const; // == size()
		bool      empty() const;
		size_type capacity() const;

		// data accessors
		reference       at(size_type pos);
		const_reference at(size_type pos) const;
		reference       front();
		const_reference front() const;
		reference       back();
		const_reference back() const;
		const_pointer   data() const;
		pointer         data();
		const_pointer   c_str() const;
		small_string    substr(size_type pos, size_type count = npos) const;

		// comparators
		int compare(const small_string& str) const;
		int compare(const CharT* str) const;
		int compare(const std::basic_string_view<CharT, Traits>& sv) const;

		// searchers
		size_type find(CharT c, size_type pos = 0) const;
		size_type find(const CharT* cstr, size_type pos, size_type count) const;
		size_type find(const CharT* cstr, size_type pos = 0) const;
		size_type find(const small_string& str, size_type pos = 0) const;
		size_type find(const std::basic_string_view<CharT, Traits>& sv, size_type pos = 0) const;
		size_type rfind(CharT c, size_type pos = npos) const;
		size_type rfind(const CharT * cstr, size_type pos, size_type count) const;
		size_type rfind(const CharT * cstr, size_type pos = npos) const;
		size_type rfind(const small_string & str, size_type pos = npos) const;
		size_type rfind(const std::basic_string_view<CharT, Traits> & sv, size_type pos = npos) const;

		// iterators
		iterator       begin();
		const_iterator begin() const;
		iterator       end();
		const_iterator end() const;

		// modifiers
		void clear();
		void reserve(size_type new_cap);
		void push_back(CharT c);
		void pop_back();
		small_string& insert(size_type index, const CharT* cstr);
		small_string& erase(size_type index, size_type count = npos);
		iterator      erase(iterator position);
		iterator      erase(iterator first, iterator last);
		small_string& append(size_type count, CharT c);
		small_string& append(const CharT* cstr, size_type count);
		small_string& append(const CharT* cstr);
		small_string& append(const small_string& str);
		small_string& append(const std::basic_string_view<CharT, Traits>& sv);
		void resize(size_type count, CharT c = CharT{});

		// conversions
		operator std::basic_string_view<CharT, Traits>() const noexcept;

		// operators
		reference       operator[](size_type pos);
		const_reference operator[](size_type pos) const;
		small_string&   operator+=(const small_string& str);
		small_string&   operator+=(const CharT* cstr);
		small_string&   operator+=(const std::basic_string_view<CharT, Traits>& sv);
		small_string&   operator+=(CharT c);

	private:
		constexpr static CharT _sso_buffer_size = (31 / sizeof(CharT));
		constexpr static CharT _use_longer_mask = 0x01;

		struct _sso;
		struct _longer;
		union _rep { _sso sso; _longer longer; };

		bool _is_sso() const;
		size_type _calc_capacity(size_type len) const;
		void _grow(size_type added_len); // grow to accomodate added len

		// compress allocator, since it's most likely size 0
		compressed_pair<_rep, allocator_type> _rep;
	};

	// non-member operators
	template<class CharT, class Traits, class Alloc>
	bool operator==(const small_string<CharT, Traits, Alloc>& lhs, const small_string<CharT, Traits, Alloc>& rhs);
	template<class CharT, class Traits, class Alloc>
	bool operator< (const small_string<CharT, Traits, Alloc>& lhs, const small_string<CharT, Traits, Alloc>& rhs);
	template<class CharT, class Traits, class Alloc>
	bool operator<=(const small_string<CharT, Traits, Alloc>& lhs, const small_string<CharT, Traits, Alloc>& rhs);
	template<class CharT, class Traits, class Alloc>
	bool operator> (const small_string<CharT, Traits, Alloc>& lhs, const small_string<CharT, Traits, Alloc>& rhs);
	template<class CharT, class Traits, class Alloc>
	bool operator>=(const small_string<CharT, Traits, Alloc>& lhs, const small_string<CharT, Traits, Alloc>& rhs);

	template<class CharT, class Traits, class Alloc>
	bool operator==(const CharT* lhs, const small_string<CharT, Traits, Alloc>& rhs);
	template<class CharT, class Traits, class Alloc>
	bool operator==(const small_string<CharT, Traits, Alloc>& lhs, const CharT* rhs);
	template<class CharT, class Traits, class Alloc>
	bool operator< (const CharT* lhs, const small_string<CharT, Traits, Alloc>& rhs);
	template<class CharT, class Traits, class Alloc>
	bool operator< (const small_string<CharT, Traits, Alloc>& lhs, const CharT* rhs);
	template<class CharT, class Traits, class Alloc>
	bool operator<=(const CharT* lhs, const small_string<CharT, Traits, Alloc>& rhs);
	template<class CharT, class Traits, class Alloc>
	bool operator<=(const small_string<CharT, Traits, Alloc>& lhs, const CharT* rhs);
	template<class CharT, class Traits, class Alloc>
	bool operator> (const CharT* lhs, const small_string<CharT, Traits, Alloc>& rhs);
	template<class CharT, class Traits, class Alloc>
	bool operator> (const small_string<CharT, Traits, Alloc>& lhs, const CharT* rhs);
	template<class CharT, class Traits, class Alloc>
	bool operator>=(const CharT* lhs, const small_string<CharT, Traits, Alloc>& rhs);
	template<class CharT, class Traits, class Alloc>
	bool operator>=(const small_string<CharT, Traits, Alloc>& lhs, const CharT* rhs);

	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(const small_string<CharT, Traits, Alloc>& lhs, const small_string<CharT, Traits, Alloc>& rhs);
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(const small_string<CharT, Traits, Alloc>& lhs, const CharT* rhs);
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(const small_string<CharT, Traits, Alloc>& lhs,	CharT rhs);
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(const CharT* lhs,                              const small_string<CharT, Traits, Alloc>& rhs);
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(CharT lhs,                                     const small_string<CharT, Traits, Alloc>& rhs);
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(small_string<CharT, Traits, Alloc>&& lhs,      small_string<CharT, Traits, Alloc>&& rhs);
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(small_string<CharT, Traits, Alloc>&& lhs,      const small_string<CharT, Traits, Alloc>& rhs);
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(small_string<CharT, Traits, Alloc>&& lhs,      const CharT* rhs);
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(small_string<CharT, Traits, Alloc>&& lhs,      CharT rhs);
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(const small_string<CharT, Traits, Alloc>& lhs, small_string<CharT, Traits, Alloc>&& rhs);
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(const CharT* lhs,                              small_string<CharT, Traits, Alloc>&& rhs);
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(CharT lhs,                                     small_string<CharT, Traits, Alloc>&& rhs);
}

// std hash overload
namespace std
{
	template<typename CharT> struct hash<idk::small_string<CharT>>
	{
		size_t operator()(const idk::small_string<CharT>& str) const noexcept
		{
			return idk::string_hash(str);
		}
	};
}

#include "small_string.inl"