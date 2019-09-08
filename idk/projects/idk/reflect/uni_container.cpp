#include "stdafx.h"
#include <reflect/reflect.h>

namespace idk::reflect
{
	uni_container::iterator::iterator(const iterator& other)
		: base{ other.base }, iter{ other.iter }
	{}

	uni_container::iterator::iterator(uni_container::base* base, dynamic iter)
		: iter{ std::move(iter) }, base{ base }
	{}

	uni_container::iterator& uni_container::iterator::operator=(const iterator& other)
	{
		base = other.base;
		iter = other.iter;
		return *this;
	}

	uni_container::iterator& uni_container::iterator::operator++() //prefix increment
	{
		base->iter_inc(*this);
		return *this;
	};

	bool uni_container::iterator::operator==(const iterator& other) const
	{
		return base->iter_cmp(*this, other);
	}

	bool uni_container::iterator::operator!=(const iterator& other) const
	{
		return !base->iter_cmp(*this, other);
	}

	dynamic uni_container::iterator::operator*() const
	{
		return std::move(base->iter_deref(*this));
	}



	size_t uni_container::size() const
	{
		return _ptr->size();
	}

	uni_container::iterator uni_container::begin() const
	{
		return _ptr->begin();
	}

	uni_container::iterator uni_container::end() const
	{
		return _ptr->end();
	}

	void uni_container::add(const dynamic& elem)
	{
		return _ptr->add(elem);
	}

	void uni_container::clear()
	{
		return _ptr->clear();
	}

    dynamic uni_container::operator[](size_t index)
    {
        return _ptr->subscript(index);
    }
    dynamic uni_container::operator[](const dynamic& key)
    {
        return _ptr->subscript(key);
    }
}