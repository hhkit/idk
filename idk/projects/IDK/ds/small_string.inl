#pragma once

#include "small_string.h"

namespace idk
{

	template<typename CharT, typename Allocator>
	small_string<CharT, Allocator>::small_string(const Allocator& alloc) noexcept
		: _rep{ {_sso{{}, _sso_buffer_size << 1}}, alloc }
	{
		static_assert(sizeof(CharT) <= 8, "just use a vector, idiot");
	}

	template<typename CharT, typename Allocator>
	small_string<CharT, Allocator>::small_string() noexcept(noexcept(Allocator))
		: small_string(Allocator{})
	{
	}

	template<typename CharT, typename Allocator>
	small_string<CharT, Allocator>::small_string(const CharT* cstr, const Allocator& alloc)
		: small_string(alloc)
	{
		auto len = std::strlen(cstr);
		auto& rep = _rep.first();

		if (len > _sso_buffer_size)
		{
			auto cap = _calc_capacity(len + 1);
			auto* buf = _rep.second().allocate(cap);
			rep.longer = _longer{ buf, len, cap };
			rep.sso.size_diff = _use_longer_mask;
			std::strncpy(buf, cstr, len + 1);
		}
		else
		{
			// mul by 2, so LSB is always 0
			rep.sso.size_diff = (_sso_buffer_size - static_cast<CharT>(len)) << 1;
			std::strncpy(rep.sso.buffer, cstr, len + 1);
		}
	}

	template<typename CharT, typename Allocator>
	template<typename InputIt>
	small_string<CharT, Allocator>::small_string(
		InputIt first, InputIt last, const Allocator& alloc)
	{
		auto len = last - first;
		auto& rep = _rep.first();

		if (len > _sso_buffer_size)
		{
			auto cap = _calc_capacity(len + 1);
			auto* buf = _rep.second().allocate(cap + 1);
			rep.alloc = _longer{ buf, len, cap };
			rep.sso.size_diff = _sso_mask;
			std::copy(first, last, buf);
			buf[len] = '\0';
		}
		else
		{
			// mul by 2, so LSB is always 0
			rep.sso.size_diff = (_sso_buffer_size - static_cast<CharT>(len)) << 1;
			std::copy(first, last, _rep.first().sso.buffer);
			rep.sso.buffer[len] = '\0';
		}
	}

	template<typename CharT, typename Allocator>
	small_string<CharT, Allocator>::~small_string()
	{
		if (!_is_sso())
			_rep.second().deallocate(_rep.first().longer.ptr, _rep.first().longer.capacity + 1);
	}



	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::size() const -> size_type
	{
		// for sso, size is stored as (bufsize - size) * 2
		return _is_sso() ? (_sso_buffer_size - (_rep.first().sso.size_diff >> 1)) : _rep.first().longer.size;
	}

	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::length() const -> size_type
	{
		return size();
	}

	template<typename CharT, typename Allocator>
	bool small_string<CharT, Allocator>::empty() const
	{
		return size() == 0;
	}

	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::capacity() const -> size_type
	{
		return _is_sso() ? _sso_buffer_size : _rep.first().longer.capacity;
	}



	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::at(size_type pos) -> reference
	{
		return begin()[pos];
	}

	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::at(size_type pos) const -> const_reference
	{
		return begin()[pos];
	}

	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::front() -> reference
	{
		return *begin();
	}

	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::front() const -> const_reference
	{
		return *begin();
	}

	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::back() -> reference
	{
		return end()[-1];
	}

	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::back() const -> const_reference
	{
		return end()[-1];
	}

	template<typename CharT, typename Allocator>
	const CharT* small_string<CharT, Allocator>::data() const
	{
		return begin();
	}

	template<typename CharT, typename Allocator>
	CharT* small_string<CharT, Allocator>::data()
	{
		return begin();
	}

	template<typename CharT, typename Allocator>
	const CharT* small_string<CharT, Allocator>::c_str() const
	{
		return begin();
	}



	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::begin() -> iterator
	{
		return _is_sso() ? _rep.first().sso.buffer : _rep.first().longer.ptr;
	}

	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::end() -> iterator
	{
		return begin() + size();
	}

	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::begin() const -> const_iterator
	{
		return _is_sso() ? _rep.first().sso.buffer : _rep.first().longer.ptr;
	}

	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::end() const -> const_iterator
	{
		return begin() + size();
	}



	template<typename CharT, typename Allocator>
	void small_string<CharT, Allocator>::clear()
	{
		auto& rep = _rep.first();
		if (_is_sso())
		{
			rep.sso.buffer[0] = '\0';
			rep.sso.size_diff = _sso_buffer_size << 1;
		}
		else
		{
			*rep.longer.ptr = '\0';
			rep.longer.size = 0;
		}
	}

	template<typename CharT, typename Allocator>
	void small_string<CharT, Allocator>::reserve(size_type new_cap)
	{
		auto* new_buf = _rep.second().allocate(new_cap + 1);
		auto* _end = end();
		std::copy(begin(), _end, new_buf);
		*_end = '\0';

		auto& rep = _rep.first();
		if (!_is_sso())
		{
			auto to_dealloc = rep.longer.ptr;
			auto cap = rep.longer.capacity;
			rep.longer = { new_buf, rep.longer.size, new_cap };
			_rep.second().deallocate(to_dealloc, cap + 1);
		}
		else
		{
			rep.longer = { new_buf, static_cast<size_t>(_sso_buffer_size - (rep.sso.size_diff >> 1)), new_cap };
			rep.sso.size_diff = _use_longer_mask;
		}
	}

	template<typename CharT, typename Allocator>
	void small_string<CharT, Allocator>::push_back(CharT c)
	{
		auto& rep = _rep.first();
		if (_is_sso())
		{
			if (rep.sso.size_diff == 0) // reached max cap
			{
				reserve(_sso_buffer_size * 2 + 1);
				rep.longer.ptr[rep.longer.size] = c;
				rep.longer.ptr[++rep.longer.size] = '\0';
				++rep.longer.size;
			}
			else
			{
				auto offset = _sso_buffer_size - (rep.sso.size_diff >> 1);
				rep.sso.buffer[offset] = c;
				rep.sso.size_diff -= 2; // -2 since we bitshift size diff by 1
				rep.sso.buffer[offset + 1] = '\0';
			}
		}
		else
		{
			if (rep.longer.size == rep.longer.capacity)
				reserve(rep.longer.capacity * 2 + 1);
			rep.longer.ptr[rep.longer.size] = c;
			rep.longer.ptr[++rep.longer.size] = '\0';
			++rep.longer.size;
		}
	}



	template<typename CharT, typename Allocator>
	bool small_string<CharT, Allocator>::_is_sso() const
	{
		return !(_rep.first().sso.size_diff & _use_longer_mask);
	}

	template<typename CharT, typename Allocator>
	auto small_string<CharT, Allocator>::_calc_capacity(size_type len) const -> size_type
	{
		--len;
		len |= len >> 1;
		len |= len >> 2;
		len |= len >> 4;
		len |= len >> 8;
		len |= len >> 16;
		return len / sizeof(CharT);
	}

}