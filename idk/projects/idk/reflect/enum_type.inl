#pragma once

#include "reflect.h"

namespace idk::reflect
{
	class enum_type::iterator
	{
	public:
		iterator(const enum_type& obj, size_t index = 0);
		iterator& operator++(); //prefix increment
		bool operator==(const iterator&);
		bool operator!=(const iterator&);
		std::pair<string_view, int64_t> operator*() const;

	private:
		const enum_type& obj;
		size_t index;
	};
}