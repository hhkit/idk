#pragma once

#include "small_string.h"
#include <ds/compressed_pair.inl>
#include <limits>
#include <algorithm>

#undef max
#undef min

namespace idk
{

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>::small_string(const Allocator& alloc) noexcept
		: _rep{ {_sso{{}, _sso_buffer_size << 1}}, alloc }
	{
		static_assert(sizeof(CharT) <= 8, "just use a vector, idiot");
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>::small_string() noexcept(noexcept(Allocator))
		: small_string(Allocator{})
	{
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>::small_string(size_type count, CharT c, const Allocator& alloc)
		: small_string(alloc)
	{
		auto& rep = _rep.first();

		if (count > _sso_buffer_size)
		{
			const auto cap = _calc_capacity(count);
			auto* buf = _rep.second().allocate(cap + 1);
			rep.longer = _longer{ buf, count, cap };
			rep.sso.size_diff = _use_longer_mask;
			for (size_type i = 0; i < count; ++i, ++buf)
				traits_type::assign(*buf, c);
			*buf = '\0';
		}
		else
		{
			// mul by 2, so LSB is always 0
			rep.sso.size_diff = (_sso_buffer_size - static_cast<CharT>(count)) << 1;
			auto* p = rep.sso.buffer;
			for (size_type i = 0; i < count; ++i, ++p)
				traits_type::assign(*p, c);
			*p = '\0';
		}
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>::small_string(const CharT* cstr, size_type count, const Allocator& alloc)
		: small_string(alloc)
	{
		auto& rep = _rep.first();

		if (count > _sso_buffer_size)
		{
			const auto cap = _calc_capacity(count);
			auto* buf = _rep.second().allocate(cap + 1);
			rep.longer = _longer{ buf, count, cap };
			rep.sso.size_diff = _use_longer_mask;
			traits_type::copy(buf, cstr, count);
            buf[count] = '\0';
		}
		else
		{
			// mul by 2, so LSB is always 0
			rep.sso.size_diff = (_sso_buffer_size - static_cast<CharT>(count)) << 1;
            if (count)
                traits_type::copy(rep.sso.buffer, cstr, count);
            rep.sso.buffer[count] = '\0';
		}
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>::small_string(const CharT* cstr, const Allocator& alloc)
		: small_string(cstr, traits_type::length(cstr), alloc)
	{
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>::small_string(const view_type& sv, const Allocator& alloc)
		: small_string(sv.data(), sv.size(), alloc)
	{
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>::small_string(const std::basic_string<CharT, Traits, Allocator>& str, const Allocator& alloc)
		: small_string(str.data(), str.size(), alloc)
	{
	}

	template<typename CharT, typename Traits, typename Allocator>
	template<typename InputIt>
	small_string<CharT, Traits, Allocator>::small_string(InputIt first, InputIt last, const Allocator& alloc)
		: small_string(alloc)
	{
		const size_type len = last - first;
		auto& rep = _rep.first();

		if (len > _sso_buffer_size)
		{
			const auto cap = _calc_capacity(len);
			auto* buf = _rep.second().allocate(cap + 1);
			rep.longer = _longer{ buf, len, cap };
			rep.sso.size_diff = _use_longer_mask;

			for (auto iter = first; iter != last; ++iter, ++buf)
				traits_type::assign(*buf, *iter);
			*buf = '\0';
		}
		else
		{
			// mul by 2, so LSB is always 0
			rep.sso.size_diff = (_sso_buffer_size - static_cast<CharT>(len)) << 1;

			auto* buf = rep.sso.buffer;
			for (auto iter = first; iter != last; ++iter, ++buf)
				traits_type::assign(*buf, *iter);
			*buf = '\0';
		}
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>::~small_string()
	{
		if (!_is_sso())
			_rep.second().deallocate(_rep.first().longer.ptr, _rep.first().longer.capacity + 1);
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>::small_string(const small_string& other)
		: small_string(other.begin(), other.end())
	{
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>::small_string(small_string&& other) noexcept
		: _rep{ other._rep }
	{
		// flag it as "using sso" so ptr is not deallocated
		other._rep.first().sso.size_diff = 0;
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::operator=(const small_string& other)
	{
		auto copy = other;
		std::swap(*this, copy);
		return *this;
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::operator=(small_string&& other) noexcept
	{
		std::swap(_rep, other._rep);
		return *this;
	}



	template<typename CharT, typename Traits, typename Allocator>
	Allocator small_string<CharT, Traits, Allocator>::get_allocator() const
	{
		return _rep.second();
	}



	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::size() const -> size_type
	{
		// for sso, size is stored as (bufsize - size) * 2
		return _is_sso() ? (_sso_buffer_size - (_rep.first().sso.size_diff >> 1)) : _rep.first().longer.size;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::length() const -> size_type
	{
		return size();
	}

	template<typename CharT, typename Traits, typename Allocator>
	bool small_string<CharT, Traits, Allocator>::empty() const
	{
		return size() == 0;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::capacity() const -> size_type
	{
		return _is_sso() ? _sso_buffer_size : _rep.first().longer.capacity;
	}



	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::at(size_type pos) -> reference
	{
		return begin()[pos];
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::at(size_type pos) const -> const_reference
	{
		return begin()[pos];
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::front() -> reference
	{
		return *begin();
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::front() const -> const_reference
	{
		return *begin();
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::back() -> reference
	{
		return end()[-1];
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::back() const -> const_reference
	{
		return end()[-1];
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::data() const -> const_pointer
	{
		return begin();
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::data() -> pointer
	{
		return begin();
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::c_str() const -> const_pointer
	{
		return begin();
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::sv() const -> view_type
	{
		return view_type(data(), size());
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator> small_string<CharT, Traits, Allocator>::substr(size_type pos, size_type count) const
	{
		return small_string(begin() + pos, _limit_count(pos, count), _rep.second());
	}



	template<typename DifferenceType, typename ValueType>
	static int _compare_sizes(ValueType a, ValueType b)
	{
		const DifferenceType d = DifferenceType(a - b);

		if (d > std::numeric_limits<int>::max())
			return std::numeric_limits<int>::max();
		else if (d < std::numeric_limits<int>::min())
			return std::numeric_limits<int>::min();
		else
			return int(d);
	}

	template<typename CharT, typename Traits, typename Allocator>
	int small_string<CharT, Traits, Allocator>::compare(const small_string& str) const
	{
		const auto s1 = size();
		const auto s2 = str.size();
		const auto cmp = traits_type::compare(data(), str.data(), (s1 < s2 ? s1 : s2));
		return cmp == 0 ? _compare_sizes<difference_type>(s1, s2) : cmp;
	}

	template<typename CharT, typename Traits, typename Allocator>
	int small_string<CharT, Traits, Allocator>::compare(const CharT* str) const
	{
		const auto s1 = size();
		const auto s2 = traits_type::length(str);
		const auto cmp = traits_type::compare(data(), str, std::min(s1, s2));
		return cmp == 0 ? _compare_sizes<difference_type>(s1, s2) : cmp;
	}

	template<typename CharT, typename Traits, typename Allocator>
	int small_string<CharT, Traits, Allocator>::compare(const view_type& sv) const
	{
		const auto s1 = size();
		const auto s2 = sv.size();
		const auto cmp = traits_type::compare(data(), sv.data(), std::min(s1, s2));
		return cmp == 0 ? _compare_sizes<difference_type>(s1, s2) : cmp;
	}



	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find(CharT c, size_type pos) const -> size_type
	{
		const auto sz = size();
		const auto _data = data();
		if (pos >= sz)
			return npos;
		auto* p = traits_type::find(_data + pos, sz - pos, c);
		return p ? p - _data : npos;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find(const CharT* cstr, size_type pos, size_type count) const -> size_type
	{
		// this function is "borrowed" from gcc :)
		const auto sz = size();

		if (pos >= sz)
			return npos;

		const auto c0 = cstr[0];
		const auto _data = data();
		auto first = _data + pos;
		const auto last = _data + sz;
		auto len = sz - pos;

		while (len >= count)
		{
			// Find the first occurrence of c0:
			first = traits_type::find(first, len - count + 1, c0);
			if (!first)
				return npos;

			// Compare the full strings from the first occurrence of c0.
			// We already know that first[0] == cstr[0] but compare them again
			// anyway because cstr is probably aligned, which helps memcmp.
			if (traits_type::compare(first, cstr, count) == 0)
				return first - _data;
			len = last - ++first;
		}

		return npos;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find(const CharT* cstr, size_type pos) const -> size_type
	{
		return find(cstr, pos, traits_type::length(cstr));
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find(const small_string& str, size_type pos) const -> size_type
	{
		return find(str.data(), pos, str.size());
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find(const view_type& sv, size_type pos) const -> size_type
	{
		return find(sv.data(), pos, sv.size());
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::rfind(CharT c, size_type pos) const -> size_type
	{
		const auto sz = size();
		const auto _data = data();
		pos = std::min(sz - 1, pos);
		if (pos >= sz)
			return npos;
		do
		{
			if (traits_type::eq(_data[pos], c))
				return pos;
		} while (pos-- > 0);
		return npos;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::rfind(const CharT* cstr, size_type pos, size_type count) const -> size_type
	{
		const auto sz = size();

		if (count > sz)
			return npos;

		pos = std::min(sz - count, pos);
		const auto _data = data();

		do
		{
			if (traits_type::compare(_data + pos, cstr, count) == 0)
				return pos;
		} while (pos-- > 0);

		return npos;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::rfind(const CharT* cstr, size_type pos) const -> size_type
	{
		return rfind(cstr, pos, traits_type::length(cstr));
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::rfind(const small_string& str, size_type pos) const -> size_type
	{
		return rfind(str.data(), pos, str.size());
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::rfind(const view_type& sv, size_type pos) const -> size_type
	{
		return rfind(sv.data(), pos, sv.size());
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find_first_of(const CharT* cstr, size_type pos, size_type count) const -> size_type
	{
		const auto _data = data();
		const auto sz = size();
		if (count == 0)
			return npos;
		for (; pos < sz; ++pos)
		{
			if (traits_type::find(cstr, count, _data[pos]))
				return pos;
		}
		return npos;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find_first_of(const CharT* cstr, size_type pos) const -> size_type
	{
		return find_first_of(cstr, pos, traits_type::length(cstr));
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find_first_of(CharT c, size_type pos) const -> size_type
	{
		const auto* p = traits_type::find(data() + pos, size() - pos, c);
		return p ? p - data() : npos;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find_first_not_of(const CharT* cstr, size_type pos, size_type count) const -> size_type
	{
		const auto _data = data();
		const auto sz = size();
		if (count == 0)
			return npos;
		for (; pos < sz; ++pos)
		{
			if (!traits_type::find(cstr, count, _data[pos]))
				return pos;
		}
		return npos;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find_first_not_of(const CharT* cstr, size_type pos) const -> size_type
	{
		return find_first_not_of(cstr, pos, traits_type::length(cstr));
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find_first_not_of(CharT c, size_type pos) const -> size_type
	{
		return find_first_not_of(&c, pos, 1);
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find_last_of(const CharT* cstr, size_type pos, size_type count) const -> size_type
	{
		const auto _data = data();
		auto sz = size();
        if (sz && count)
        {
            if (--sz > pos)
                sz = pos;
            do
            {
                if (traits_type::find(cstr, count, _data[sz]))
                    return sz;
            }
            while (sz-- != 0);
        }
		return npos;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find_last_of(const CharT* cstr, size_type pos) const -> size_type
	{
		return find_last_of(cstr, pos, traits_type::length(cstr));
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find_last_of(CharT c, size_type pos) const -> size_type
	{
		return find_last_of(&c, pos, 1);
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find_last_not_of(const CharT* cstr, size_type pos, size_type count) const -> size_type
	{
        const auto _data = data();
        auto sz = size();
        if (sz && count)
        {
            if (--sz > pos)
                sz = pos;
            do
            {
                if (!traits_type::find(cstr, count, _data[sz]))
                    return sz;
            } while (sz-- != 0);
        }
        return npos;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find_last_not_of(const CharT* cstr, size_type pos) const -> size_type
	{
		return find_last_not_of(cstr, pos, traits_type::length(cstr));
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::find_last_not_of(CharT c, size_type pos) const -> size_type
	{
		return find_last_not_of(&c, pos, 1);
	}



	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::begin() -> iterator
	{
		return _is_sso() ? _rep.first().sso.buffer : _rep.first().longer.ptr;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::begin() const -> const_iterator
	{
		return _is_sso() ? _rep.first().sso.buffer : _rep.first().longer.ptr;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::end() -> iterator
	{
		return begin() + size();
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::end() const -> const_iterator
	{
		return begin() + size();
	}



	template<typename CharT, typename Traits, typename Allocator>
	void small_string<CharT, Traits, Allocator>::clear()
	{
		auto& rep = _rep.first();
		if (_is_sso())
		{
			*rep.sso.buffer = '\0';
			rep.sso.size_diff = _sso_buffer_size << 1;
		}
		else
		{
			*rep.longer.ptr = '\0';
			rep.longer.size = 0;
		}
	}

	template<typename CharT, typename Traits, typename Allocator>
	void small_string<CharT, Traits, Allocator>::reserve(size_type new_cap)
	{
		auto* new_buf = _rep.second().allocate(new_cap + 1);
		const auto _end = end();
		std::copy(begin(), _end + 1, new_buf);

		auto& rep = _rep.first();
		if (!_is_sso())
		{
			const auto to_dealloc = rep.longer.ptr;
			const auto cap = rep.longer.capacity;
			rep.longer = { new_buf, rep.longer.size, new_cap };
			_rep.second().deallocate(to_dealloc, cap + 1);
		}
		else
		{
			rep.longer = { new_buf, static_cast<size_type>(_sso_buffer_size - (rep.sso.size_diff >> 1)), new_cap };
			rep.sso.size_diff = _use_longer_mask;
		}
	}

	template<typename CharT, typename Traits, typename Allocator>
	void small_string<CharT, Traits, Allocator>::push_back(CharT c)
	{
		auto& rep = _rep.first();
		_grow(1);

		if (_is_sso())
		{
			const auto offset = _sso_buffer_size - (rep.sso.size_diff >> 1);
			traits_type::assign(rep.sso.buffer[offset], c);
			rep.sso.size_diff -= 2; // -2 since we bitshift size diff by 1
			rep.sso.buffer[offset + 1] = '\0';
		}
		else
		{
			traits_type::assign(rep.longer.ptr[rep.longer.size], c);
			rep.longer.ptr[++rep.longer.size] = '\0';
		}
	}

	template<typename CharT, typename Traits, typename Allocator>
	void small_string<CharT, Traits, Allocator>::pop_back()
	{
		if (_is_sso())
			_rep.first().sso.buffer[_sso_buffer_size - ((_rep.first().sso.size_diff += 2) >> 1)] = '\0';
		else
			_rep.first().longer.ptr[--_rep.first().longer.size] = '\0';
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::insert(size_type index, const CharT* cstr)
	{
		auto& rep = _rep.first();
		const auto len = traits_type::length(cstr);
		_grow(len);

		if (_is_sso())
		{
            traits_type::move(rep.sso.buffer + index + len, rep.sso.buffer + index, _sso_buffer_size - (rep.sso.size_diff >> 1) - index - len + 1);
			// write size_diff first as it might be overwritten during copy
			rep.sso.size_diff -= static_cast<CharT>(len << 1);
			traits_type::copy(rep.sso.buffer + index, cstr, len); // \0 also right shifted, dont have to write it
		}
		else
		{
            traits_type::move(rep.longer.ptr + index + len, rep.longer.ptr + index, rep.longer.size - index - len + 1);
			traits_type::copy(rep.longer.ptr + index, cstr, len);
			rep.longer.size += len;
		}

		return *this;
	}

    template<typename CharT, typename Traits, typename Allocator>
    small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::insert(size_type index, size_type count, CharT c)
    {
        auto& rep = _rep.first();
        _grow(count);

        if (_is_sso())
        {
            traits_type::move(rep.sso.buffer + index + count, rep.sso.buffer + index, _sso_buffer_size - (rep.sso.size_diff >> 1) - index - count + 1);
            // write size_diff first as it might be overwritten during copy
            rep.sso.size_diff -= static_cast<CharT>(count << 1);
            traits_type::assign(rep.sso.buffer + index, count, c); // \0 also right shifted, dont have to write it
        }
        else
        {
            traits_type::move(rep.longer.ptr + index + count, rep.longer.ptr + index, rep.longer.size - index - count + 1);
            traits_type::assign(rep.longer.ptr + index, count, c);
            rep.longer.size += count;
        }

        return *this;
    }

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::erase(size_type index, size_type count)
	{
		auto& rep = _rep.first();

		if (_is_sso())
		{
			auto sz = _sso_buffer_size - (rep.sso.size_diff >> 1);
			count = std::min(count, sz - index);
            traits_type::move(rep.sso.buffer + index, rep.sso.buffer + index + count, sz - index - count + 1);
			rep.sso.size_diff += static_cast<CharT>(count << 1);
		}
		else
		{
			count = std::min(count, rep.longer.size - index);
            traits_type::move(rep.longer.ptr + index, rep.longer.ptr + index + count, rep.longer.size - index - count + 1);
            
			_rep.first().longer.size -= count;
		}

		return *this;
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::erase(iterator position) -> iterator
	{
		return erase(position, position + 1);
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::erase(iterator first, iterator last) -> iterator
	{
		const auto count = last - first;
        traits_type::move(first, last, size() - (last - begin()) + 1); // +1 for \0
		if (_is_sso()) _rep.first().sso.size_diff += static_cast<CharT>(count) << 1;
		else           _rep.first().longer.size -= count;
		return first;
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::append(size_type count, CharT c)
	{
		if (count == 0)
			return *this;

		auto& rep = _rep.first();
		_grow(count);

		if (_is_sso())
		{
			auto p = rep.sso.buffer + _sso_buffer_size - (rep.sso.size_diff >> 1);
			// write size_diff first as it might be overwritten during copy
			rep.sso.size_diff -= static_cast<CharT>(count << 1);
			for (auto i = 0; i < count; ++i, ++p)
				traits_type::assign(*p, c);
			*p = '\0';
		}
		else
		{
			auto p = rep.longer.ptr + rep.longer.size;
			for (auto i = 0; i < count; ++i, ++p)
				traits_type::assign(*p, c);
			*p = '\0';
			rep.longer.size += count;
		}

		return *this;
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::append(const CharT* cstr, size_type count)
	{
		auto& rep = _rep.first();
		_grow(count);

		if (_is_sso())
		{
			const auto size_diff = rep.sso.size_diff;
			// write size_diff first as it might be overwritten during copy
			rep.sso.size_diff -= static_cast<CharT>(count << 1);
			traits_type::copy(rep.sso.buffer + _sso_buffer_size - (size_diff >> 1), cstr, count + 1);
		}
		else
		{
			traits_type::copy(rep.longer.ptr + rep.longer.size, cstr, count + 1);
			rep.longer.size += count;
		}

		return *this;
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::append(const CharT* cstr)
	{
		return append(cstr, traits_type::length(cstr));
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::append(const small_string& str)
	{
		return append(str.data(), str.size());
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::append(const std::basic_string_view<CharT, Traits>& sv)
	{
		return append(sv.data(), sv.size());
	}

    template<typename CharT, typename Traits, typename Allocator>
    small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::replace(size_type position, size_type count, const CharT* cstr, size_type count2)
    {
        const size_type old_sz = size();
        const size_type new_sz = old_sz + count2 - count;
        const size_type right_sz = old_sz - position - count;

        if (new_sz <= capacity())
        {
            pointer p = data() + position;

            if (cstr < data() || end() < cstr) // does not overlap
            {
                if (right_sz && count != count2)
                    traits_type::move(p + count2, p + count, right_sz);
                if (count2)
                    traits_type::copy(p, cstr, count2);
            }
            else
            {
                // Work in-place.
                if (count2 && count2 <= count)
                    traits_type::move(p, cstr, count2);
                if (right_sz && count != count2)
                    traits_type::move(p + count2, p + count, right_sz);
                if (count2 > count)
                {
                    if (cstr + count2 <= p + count)
                        traits_type::move(p, cstr, count2);
                    else if (cstr >= p + count)
                        traits_type::copy(p, cstr + count2 - count, count2);
                    else
                    {
                        const size_type nleft = (p + count) - cstr;
                        traits_type::move(p, cstr, nleft);
                        traits_type::copy(p + nleft, p + count2, count2 - nleft);
                    }
                }
            }

            _set_size(new_sz);
        }
        else
        {
            // reserve and copy manually
            size_type new_cap = _calc_capacity(new_sz);
            auto* new_buf = _rep.second().allocate(new_cap + 1);
            const auto _beg = begin();
            std::copy(_beg, _beg + position, new_buf);
            traits_type::copy(new_buf + position, cstr, count2);
            traits_type::copy(new_buf + position + count2, _beg + position + count, right_sz);

            auto& rep = _rep.first();
            if (!_is_sso())
            {
                const auto to_dealloc = rep.longer.ptr;
                const auto cap = rep.longer.capacity;
                rep.longer = { new_buf, new_sz, new_cap };
                _rep.second().deallocate(to_dealloc, cap + 1);
            }
            else
            {
                rep.longer = { new_buf, new_sz, new_cap };
                rep.sso.size_diff = _use_longer_mask;
            }
        }

        return *this;
    }

    template<typename CharT, typename Traits, typename Allocator>
    small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::replace(size_type position, size_type count, const CharT* cstr)
    {
        return replace(position, count, cstr, traits_type::length(cstr));
    }

    template<typename CharT, typename Traits, typename Allocator>
    small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::replace(size_type position, size_type count, view_type sv)
    {
        return replace(position, count, sv.data(), sv.size());
    }

    template<typename CharT, typename Traits, typename Allocator>
    small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::replace(size_type position, size_type count, size_type count2, CharT c)
    {
        const size_type old_sz = size();
        const size_type new_sz = old_sz + count2 - count;
        const size_type right_sz = old_sz - position - count;

        if (new_sz <= capacity())
        {
            pointer p = data() + position;

            if (right_sz && count != count2)
                traits_type::move(p + count2, p + count, right_sz);
            if (count2)
                traits_type::assign(p, count2, c);

            _set_size(new_sz);
        }
        else
        {
            // reserve and copy manually
            size_type new_cap = _calc_capacity(new_sz);
            auto* new_buf = _rep.second().allocate(new_cap + 1);
            const auto _beg = begin();
            std::copy(_beg, _beg + position, new_buf);
            traits_type::assign(new_buf + position, count2, c);
            traits_type::copy(new_buf + position + count2, _beg + position + count, right_sz);

            auto& rep = _rep.first();
            if (!_is_sso())
            {
                const auto to_dealloc = rep.longer.ptr;
                const auto cap = rep.longer.capacity;
                rep.longer = { new_buf, new_sz, new_cap };
                _rep.second().deallocate(to_dealloc, cap + 1);
            }
            else
            {
                rep.longer = { new_buf, new_sz, new_cap };
                rep.sso.size_diff = _use_longer_mask;
            }
        }

        return *this;
    }

	template<typename CharT, typename Traits, typename Allocator>
	void small_string<CharT, Traits, Allocator>::resize(size_type count, CharT c)
	{
		const auto sz = size();
		if (count < sz)
			erase(count);
		else
			append(count - sz, c);
	}



	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>::operator view_type() const noexcept
	{
		return sv();
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>::operator std::basic_string<CharT, Traits, Allocator>() const
	{
		return std::basic_string<CharT, Traits, Allocator>(data(), size());
	}



	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::operator[](size_type pos) -> reference
	{
		return at(pos);
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::operator[](size_type pos) const -> const_reference
	{
		return at(pos);
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::operator+=(const small_string& str)
	{
		return append(str);
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::operator+=(const CharT* cstr)
	{
		return append(cstr);
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::operator+=(const std::basic_string_view<CharT, Traits>& sv)
	{
		return append(sv);
	}

	template<typename CharT, typename Traits, typename Allocator>
	small_string<CharT, Traits, Allocator>& small_string<CharT, Traits, Allocator>::operator+=(CharT c)
	{
		push_back(c);
		return *this;
	}



	template<typename CharT, typename Traits, typename Allocator>
	bool small_string<CharT, Traits, Allocator>::_is_sso() const
	{
		return !(_rep.first().sso.size_diff & _use_longer_mask);
	}

	template<typename CharT, typename Traits, typename Allocator>
	auto small_string<CharT, Traits, Allocator>::_calc_capacity(size_type len) const -> size_type
	{
		len |= len >> 1;
		len |= len >> 2;
		len |= len >> 4;
		len |= len >> 8;
		len |= len >> 16;
		return len;
	}

	template<typename CharT, typename Traits, typename Allocator>
	void small_string<CharT, Traits, Allocator>::_grow(size_type added_len)
	{
		auto& rep = _rep.first();
		if (_is_sso())
		{
			auto size_diff = rep.sso.size_diff >> 1;
			if (size_diff < added_len) // not enough space in sso
				reserve(_calc_capacity(_sso_buffer_size - size_diff + added_len));
		}
		else if(rep.longer.capacity < rep.longer.size + added_len)
			reserve(_calc_capacity(rep.longer.size + added_len));
	}

    template<typename CharT, typename Traits, typename Allocator>
    void small_string<CharT, Traits, Allocator>::_set_size(size_type sz)
    {
        if (_is_sso())
            _rep.first().sso.size_diff = (_sso_buffer_size - static_cast<CharT>(sz)) << 1;
        else
            _rep.first().longer.size = sz;
    }

    template<typename CharT, typename Traits, typename Allocator>
    auto small_string<CharT, Traits, Allocator>::_limit_count(size_type pos, size_type count) const -> size_type
    {
        return count < size() - pos ?
            count : size() - pos;
    }



	template<class CharT, class Traits, class Alloc>
	bool operator==(const small_string<CharT, Traits, Alloc>& lhs, const small_string<CharT, Traits, Alloc>& rhs)
	{
		return lhs.compare(rhs) == 0;
	}
	template<class CharT, class Traits, class Alloc>
	bool operator< (const small_string<CharT, Traits, Alloc>& lhs, const small_string<CharT, Traits, Alloc>& rhs)
	{
		return lhs.compare(rhs) < 0;
	}
	template<class CharT, class Traits, class Alloc>
	bool operator<=(const small_string<CharT, Traits, Alloc>& lhs, const small_string<CharT, Traits, Alloc>& rhs)
	{
		return lhs.compare(rhs) <= 0;
	}
	template<class CharT, class Traits, class Alloc>
	bool operator> (const small_string<CharT, Traits, Alloc>& lhs, const small_string<CharT, Traits, Alloc>& rhs)
	{
		return lhs.compare(rhs) > 0;
	}
	template<class CharT, class Traits, class Alloc>
	bool operator>=(const small_string<CharT, Traits, Alloc>& lhs, const small_string<CharT, Traits, Alloc>& rhs)
	{
		return lhs.compare(rhs) >= 0;
	}



	template<class CharT, class Traits, class Alloc>
	bool operator==(const CharT* lhs, const small_string<CharT, Traits, Alloc>& rhs)
	{
		return rhs.compare(lhs) == 0;
	}
	template<class CharT, class Traits, class Alloc>
	bool operator==(const small_string<CharT, Traits, Alloc>& lhs, const CharT* rhs)
	{
		return lhs.compare(rhs) == 0;
	}

	template<class CharT, class Traits, class Alloc>
	bool operator< (const CharT* lhs, const small_string<CharT, Traits, Alloc>& rhs)
	{
		return rhs.compare(lhs) > 0;
	}
	template<class CharT, class Traits, class Alloc>
	bool operator< (const small_string<CharT, Traits, Alloc>& lhs, const CharT* rhs)
	{
		return lhs.compare(rhs) < 0;
	}

	template<class CharT, class Traits, class Alloc>
	bool operator<=(const CharT* lhs, const small_string<CharT, Traits, Alloc>& rhs)
	{
		return rhs.compare(lhs) >= 0;
	}
	template<class CharT, class Traits, class Alloc>
	bool operator<=(const small_string<CharT, Traits, Alloc>& lhs, const CharT* rhs)
	{
		return lhs.compare(rhs) <= 0;
	}

	template<class CharT, class Traits, class Alloc>
	bool operator> (const CharT* lhs, const small_string<CharT, Traits, Alloc>& rhs)
	{
		return rhs.compare(lhs) < 0;
	}
	template<class CharT, class Traits, class Alloc>
	bool operator> (const small_string<CharT, Traits, Alloc>& lhs, const CharT* rhs)
	{
		return lhs.compare(rhs) > 0;
	}

	template<class CharT, class Traits, class Alloc>
	bool operator>=(const CharT* lhs, const small_string<CharT, Traits, Alloc>& rhs)
	{
		return rhs.compare(lhs) <= 0;
	}
	template<class CharT, class Traits, class Alloc>
	bool operator>=(const small_string<CharT, Traits, Alloc>& lhs, const CharT* rhs)
	{
		return lhs.compare(rhs) >= 0;
	}
	


	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(const small_string<CharT, Traits, Alloc>& lhs, const small_string<CharT, Traits, Alloc>& rhs)
	{
		return small_string(lhs).append(rhs);
	}
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(const small_string<CharT, Traits, Alloc>& lhs, const CharT* rhs)
	{
		return small_string(lhs).append(rhs);
	}
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(const small_string<CharT, Traits, Alloc>& lhs,	CharT rhs)
	{
        auto copy = small_string(lhs);
        copy.push_back(rhs);
        return copy;
	}
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(const CharT* lhs,                              const small_string<CharT, Traits, Alloc>& rhs)
	{
		return small_string(lhs).append(rhs);
	}
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(CharT lhs,                                     const small_string<CharT, Traits, Alloc>& rhs)
	{
		return small_string(1, lhs).append(rhs);
	}
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(small_string<CharT, Traits, Alloc>&& lhs,      small_string<CharT, Traits, Alloc>&& rhs)
	{
		return lhs.append(rhs);
	}
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(small_string<CharT, Traits, Alloc>&& lhs,      const small_string<CharT, Traits, Alloc>& rhs)
	{
		return lhs.append(rhs);
	}
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(small_string<CharT, Traits, Alloc>&& lhs,      const CharT* rhs)
	{
		return lhs.append(rhs);
	}
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(small_string<CharT, Traits, Alloc>&& lhs,      CharT rhs)
	{
		lhs.push_back(rhs);
        return lhs;
	}
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(const small_string<CharT, Traits, Alloc>& lhs, small_string<CharT, Traits, Alloc>&& rhs)
	{
		return small_string(lhs).append(rhs);
	}
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(const CharT* lhs,                              small_string<CharT, Traits, Alloc>&& rhs)
	{
		return small_string(lhs).append(rhs);
	}
	template<class CharT, class Traits, class Alloc>
	small_string<CharT, Traits, Alloc> operator+(CharT lhs,                                     small_string<CharT, Traits, Alloc>&& rhs)
	{
		return small_string(1, lhs).append(rhs);
	}



	template<class CharT, class Traits, class Allocator>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const small_string<CharT, Traits, Allocator>& str)
	{
		return os << str.sv();
	}
	template<class CharT, class Traits, class Allocator>
	std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, small_string<CharT, Traits, Allocator>& str)
	{
		typename std::basic_istream<CharT, Traits>::sentry s{ is };
		if (s)
		{
			str.clear();
			auto w = is.width();
			for (std::streamsize i = 0; i < w; ++i)
			{
                char c = is.get();
				if (is.eof() || std::isspace(c, is.getloc()))
					break;
				str.push_back(c);
			}
		}
        return is;
	}
}